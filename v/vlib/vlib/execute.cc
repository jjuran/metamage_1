/*
	execute.cc
	----------
*/

#include "vlib/execute.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/assert.hh"
#include "vlib/collectible.hh"
#include "vlib/eval.hh"
#include "vlib/exceptions.hh"
#include "vlib/in-flight.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/symdesc.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/generic_iterator.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/invocation.hh"
#include "vlib/types/lambda.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/string.hh"
#include "vlib/types/symdesc.hh"
#include "vlib/types/term.hh"


namespace vlib
{
	
	class gc_cleanup
	{
		private:
			const Value& its_symbol_list;
			
			// non-copyable
			gc_cleanup           ( const gc_cleanup& );
			gc_cleanup& operator=( const gc_cleanup& );
		
		public:
			gc_cleanup( const Value& symlist ) : its_symbol_list( symlist )
			{
			}
			
			~gc_cleanup();
	};
	
	gc_cleanup::~gc_cleanup()
	{
		if ( symbol_list_with_values_is_collectible( its_symbol_list ) )
		{
			cull_unreachable_objects();
		}
	}
	
	static
	Value gensym( symbol_type type, const plus::string& name, const Value &v )
	{
		const Variable symbol( name );
		
		/*
			`_` doesn't need to be tracked, because it's never closed over
			and therefore can't be involved in a reference loop.
		*/
		
		symbol.sym()->deref_unsafe() = v;
		
		return symbol;
	}
	
	static
	Value unshare_symbols( const Value& presets )
	{
		if ( is_empty_list( presets ) )
		{
			return Value_empty_list;
		}
		
		const Value& head = first( presets );
		const Value& tail = rest ( presets );
		
		const Symbol* sym = head.sym();
		
		const Value new_head = sym->is_immutable() ? head : sym->clone();
		
		if ( is_empty_list( tail ) )
		{
			return new_head;
		}
		
		return Value( new_head, unshare_symbols( tail ) );
	}
	
	static
	Value_in_flight execute( const Value& tree, const Value& stack );
	
	static
	Value_in_flight invoke_block( const Value& block, const Value& arguments )
	{
		Expr* expr = block.expr();
		
		ASSERT( expr );
		
		ASSERT( expr->op == Op_activation );
		
		const Value& stack = expr->left;
		const Value& entry = expr->right;
		
		expr = stack.expr();
		
		ASSERT( expr );
		
		ASSERT( expr->op == Op_frame );
		
		const Value& caller = expr->left;
		const Value& locals = expr->right;
		
		const bool is_call = arguments.type();
		
		const Value underscore = is_call ? gensym( Symbol_var, "_", arguments )
		                                 : first( locals );
		
		const Value new_frame( underscore, unshare_symbols( rest( locals ) ) );
		const Value new_stack( caller, Op_frame, new_frame );
		
		const Value& unshared_locals = is_call ? new_frame
		                                       : new_frame.expr()->right;
		
		gc_cleanup gc( unshared_locals );
		
		scoped_root scope( new_stack );
		
		return execute( entry, new_stack );
	}
	
	static
	Value v_invoke( const Value& v )
	{
		scoped_root scope( v );
		
		Expr* expr = v.expr();
		
		ASSERT( expr != 0 );  // NULL
		
		if ( expr->op == Op_bind_args )
		{
			const Value& block = expr->left;
			const Value& args  = expr->right;
			
			return invoke_block( block, args );
		}
		
		if ( expr->op == Op_activation )
		{
			return invoke_block( v, NIL );
		}
		
		ASSERT( expr->op == Op_expression );
		
		return execute( expr->right, expr->left );
	}
	
	static proc_info proc_invoke = { "invoke", &v_invoke, 0 };
	
	static const Proc invoke_proc( proc_invoke );
	
	static
	Value make_block_invocation( const Value& scope, const Value& stack )
	{
		const Expr* expr = scope.expr();
		
		ASSERT( expr );
		
		ASSERT( expr->op == Op_scope );
		
		const Value& data = expr->left;   // args and local symbols
		const Value& code = expr->right;  // expression tree
		
		const Value& underscore = first( stack.expr()->right );
		
		const Value new_frame( underscore, rest( data ) );
		
		const Value new_stack( stack, Op_frame, new_frame );
		const Value activation( new_stack, Op_activation, code );
		
		return Invocation( invoke_proc, activation );
	}
	
	static
	Value invocable_expression( const Value& tree, const Value& stack )
	{
		const Value expr( stack, Op_expression, tree );
		
		return Invocation( invoke_proc, expr );
	}
	
	class temporary
	{
		private:
			Value& its_location;
			
			// non-copyable
			temporary           ( const temporary& );
			temporary& operator=( const temporary& );
		
		public:
			temporary( Value& location )
			:
				its_location( location )
			{
			}
			
			~temporary()
			{
				its_location = undefined;
			}
	};
	
	static
	void run_for_loop( const Value& do_clause, const Value& stack )
	{
		/*
			right operand is `x in container do {...}`:
			
			for
						x
					in
						container
				do
					{...}
		*/
		
		Expr* expr = do_clause.expr();
		
		if ( expr == 0  ||  expr->op != Op_do_2 )  // NULL
		{
			THROW( "`for` expects `do`" );
		}
		
		const Value& in_clause = expr->left;
		const Value& do_block = expr->right;
		
		expr = in_clause.expr();
		
		if ( expr == 0  ||  expr->op != Op_in )  // NULL
		{
			THROW( "`for` expects `in`" );
		}
		
		const Value& declarate = expr->left;
		
		Value_in_flight container = execute( expr->right, stack );
		
		int index = 0;
		
		if ( Symbol* sym = declarate.sym() )
		{
			const Value& v = sym->get();
			
			ASSERT( v.type() == V_desc );
			
			const SymDesc& symdesc = (const SymDesc&) v;
			
			index = symdesc.index();
			
			ASSERT( symdesc.depth() == 0 );
		}
		else
		{
			THROW( "`for` expects an iteration symbol" );
		}
		
		Value invocation = execute( do_block, stack );
		
		invocation.unshare();
		
		expr = invocation.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_invocation );
		
		
		Value& activation = expr->right;
		
		activation.unshare();
		
		expr = activation.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_activation );
		
		
		Value& stack_head = expr->left;
		
		stack_head.unshare();
		
		expr = stack_head.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_frame );
		
		
		Value& stack_next = expr->left;
		
		stack_next.unshare();
		
		expr = stack_next.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_frame );
		
		
		Value& stack_frame = expr->right;
		
		Value& variable = get_nth_mutable( stack_frame, index );
		
		if ( const dispatch* methods = container.get().dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_1arg handler = ops->unary )
				{
					while ( true )
					{
						// This may block
						
						Value& result = variable.sym()->deref_unsafe();
						
						result = handler( Op_recv, container );
						
						if ( result.type() == 0 )
						{
							break;
						}
						
						if ( is_empty_list( result ) )
						{
							return;
						}
						
						temporary tmp( result );
						
						try
						{
							v_invoke( activation );
						}
						catch ( const transfer_via_break& )
						{
							return;
						}
						catch ( const transfer_via_continue& )
						{
							continue;  // no-op
						}
					}
				}
			}
		}
		
		generic_iterator it( container );
		
		while ( it )
		{
			variable.sym()->deref_unsafe() = it.use();
			
			try
			{
				v_invoke( activation );
			}
			catch ( const transfer_via_break& )
			{
				break;
			}
			catch ( const transfer_via_continue& )
			{
				continue;  // no-op
			}
		}
	}
	
	static
	void test_assertion( const Expr* expr, const Value& stack )
	{
		const Value& test = expr->right;
		
		const Value result = execute( test, stack );
		
		check_assertion_result( test, result, expr->source );
	}
	
	static
	Value short_circuit_logic( const Expr* expr, bool bail, const Value& stack )
	{
		const Value& test = expr->left;
		
		const Value result = execute( expr->left, stack );
		
		const bool truth = result.to< Boolean >();
		
		if ( truth == bail )
		{
			return result;
		}
		
		return execute( expr->right, stack );
	}
	
	static
	void export_symbol( const Value& exports, const Value& symbol )
	{
		Symbol& xsym = *exports.sym();
		
		if ( Expr* expr = xsym.get().expr() )
		{
			ASSERT( expr->op == Op_export );
			ASSERT( is_array( expr->right ) );
			
			Target target = { &expr->right, &NIL };
			
			push( target, symbol );
			
			return;
		}
		
		ASSERT( xsym.get().type() == Value_undefined );
		
		xsym.deref() = Value( Op_export, symbol );
	}
	
	static
	Value resolve_symbol_expr( const Value& v, const Value& stack )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( declares_symbols( expr->op ) )
			{
				return resolve_symbol_expr( expr->right, stack );
			}
			
			if ( expr->op == Op_denote )
			{
				const Value& left = resolve_symbol( expr->left, stack );
				
				const Value type = execute( expr->right, stack );
				
				return eval( left, Op_denote, type, expr->source );
			}
			
			if ( expr->op == Op_move )
			{
				return execute( v, stack );
			}
			
			if ( expr->op == Op_export )
			{
				const Value& right = resolve_symbol( expr->right, stack );
				
				export_symbol( expr->left, right );
				
				return right;
			}
			
			if ( expr->op == Op_unary_deref )
			{
				const Value reference = execute( expr->right, stack );
				
				if ( Expr* rexpr = reference.expr() )
				{
					return rexpr->right;
				}
			}
			
			const bool exec = expr->op != Op_list;
			
			return Value( resolve_symbol_expr( expr->left, stack ),
			              expr->op,
			              exec ? execute            ( expr->right, stack ).get()
			                   : resolve_symbol_expr( expr->right, stack ) );
		}
		
		if ( v.type() != Value_symbol  &&  ! is_etc( v ) )
		{
			THROW( "mutable operand must be a symbol (or component thereof)" );
		}
		
		return resolve_symbol( v, stack );
	}
	
	static
	Value v_define( const Value& v )
	{
		const Value& name = first( v );
		const Value& body = rest ( v );
		
		Expr* expr = body.expr();
		
		if ( expr  &&  expr->op == Op_prototype )
		{
			expr = expr->right.expr();
		}
		
		if ( expr == 0  ||  expr->op != Op_invocation )  // NULL
		{
			THROW( "`def` requires a block" );
		}
		
		Lambda lambda( body );
		
		eval( name, Op_duplicate, lambda, expr->source );
		
		return lambda;
	}
	
	static const proc_info proc_define = { "define", &v_define, 0 };  // NULL
	
	static const Proc define_proc( proc_define );
	
	static
	bool is_elseif( const Expr* expr )
	{
		if ( expr->op == Op_else )
		{
			if (( expr = expr->right.expr() ))
			{
				return expr->op == Op_if;
			}
		}
		
		return false;
	}
	
	Value_in_flight execute( const Value& tree, const Value& stack )
	{
		const Value* next = &tree;
		
	tail_call:
		
		if ( next->is_evaluated() )
		{
			return *next;
		}
		
		const Value& subtree = *next;
		
		if ( Expr* expr = subtree.expr() )
		{
			if ( expr->op == Op_for )
			{
				run_for_loop( expr->right, stack );
				
				return Value();
			}
			
			if ( expr->op == Op_module )
			{
				return Value();
			}
			
			if ( declares_symbols( expr->op ) )
			{
				if ( expr->right.type() != Value_symbol )
				{
					THROW( "declarator operand must be a symbol" );
				}
				
				const Value& resolved = resolve_symbol( expr->right, stack );
				
				if ( expr->op == Op_def )
				{
					return bind_args( define_proc, resolved );
				}
				
				return resolved;
			}
			
			if ( expr->op == Op_export )
			{
				const Value& symbol = expr->right;
				
				export_symbol( expr->left, resolve_symbol( symbol, stack ) );
				
				next = &symbol;
				goto tail_call;
			}
			
			if ( expr->op == Op_end )
			{
				execute( expr->left, stack );
				
				next = &expr->right;
				goto tail_call;
			}
			
			if ( expr->op == Op_block )
			{
				const Value& scope = expr->right;
				
				return make_block_invocation( scope, stack );
			}
			
			if ( expr->op == Op_do_2 )
			{
				const Value& test = invocable_expression( expr->left, stack );
				
				return Value( test, Op_do_2, execute( expr->right, stack ) );
			}
			
			if ( expr->op == Op_while_2  ||  is_elseif( expr ) )
			{
				const Value& test = invocable_expression( expr->right, stack );
				
				return Value( execute( expr->left, stack ), expr->op, test );
			}
			
			if ( expr->op == Op_assert )
			{
				test_assertion( expr, stack );
				
				return nothing;
			}
			
			if ( expr->op == Op_and  ||  expr->op == Op_or )
			{
				bool bail_on_truth = expr->op == Op_or;
				
				return short_circuit_logic( expr, bail_on_truth, stack );
			}
			
			if ( expr->op == Op_unary_refer )
			{
				const Value& v = expr->right;
				
				return Value( Op_unary_refer, resolve_symbol_expr( v, stack ) );
			}
			
			const Value* left  = &expr->left;
			const Value* right = &expr->right;
			
			if ( left->type() == Value_dummy_operand )
			{
				using iota::swap;
				
				swap( left, right );
			}
			else if ( expr->op == Op_typeof )
			{
				// This is an endec type, not a typeof operation
				
				return tree;
			}
			
			if ( is_right_varop( expr->op ) )
			{
				return eval( resolve_symbol_expr( *left, stack ),
				             expr->op,
				             resolve_symbol_expr( *right, stack ),
				             expr->source );
			}
			
			if ( is_left_varop( expr->op )  &&  ! is_type_annotation( *left ) )
			{
				return eval( resolve_symbol_expr( *left, stack ),
				             expr->op,
				             execute( *right, stack ),
				             expr->source );
			}
			
			/*
				WARNING:  This recursion is unsafe.  It can be abused with a
				sufficiently long program, and recursive user-defined blocks
				only make this easier.
			*/
			
			return eval( execute( *left, stack ),
			             expr->op,
			             execute( *right, stack ),
			             expr->source );
		}
		
		const Value& resolved = resolve_symbol( subtree, stack );
		
		return eval( resolved );
	}
	
	Value execute( const Value& root )
	{
		scoped_root scope( root );
		
		Expr* expr = root.expr();
		
		const Value stack( NIL, Op_frame, expr->left );
		
		try
		{
			return execute( expr->right, stack );
		}
		catch ( const exception& e )
		{
			throw user_exception( String( e.message ), expr->source );
		}
		
		// Silence Metrowerks C++ warning
		return Value();
	}
	
}
