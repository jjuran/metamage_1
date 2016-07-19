/*
	execute.cc
	----------
*/

#include "vlib/execute.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/eval.hh"
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/peephole.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/symdesc.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"
#include "vlib/type_info.hh"
#include "vlib/iterators/generic_iterator.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/boolean.hh"


namespace vlib
{
	
	static
	language_error assertion_result_not_boolean( const source_spec& source )
	{
		return language_error( "assertion result not boolean", source );
	}
	
	static
	Value gensym( symbol_type type, const plus::string& name, const Value &v )
	{
		const Value symbol( Symbol_var, name );
		
		track_symbol( symbol, v );
		
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
	Value execute( const Value& tree, const Value& stack );
	
	static
	Value invoke_block( const Value& block, const Value& arguments )
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
		
		return execute( entry, new_stack );
	}
	
	static
	Value v_invoke( const Value& v )
	{
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
			return invoke_block( v, nothing );
		}
		
		ASSERT( expr->op == Op_expression );
		
		return execute( expr->right, expr->left );
	}
	
	static proc_info proc_invoke = { "invoke", &v_invoke, 0 };
	
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
		
		return Value( proc_invoke, Op_invocation, activation );
	}
	
	static
	Value invocable_expression( const Value& tree, const Value& stack )
	{
		const Value expr( stack,       Op_expression, tree );
		const Value proc( proc_invoke, Op_invocation, expr );
		
		return proc;
	}
	
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
		
		Value container = execute( expr->right, stack );
		
		int index = 0;
		
		if ( Symbol* sym = declarate.sym() )
		{
			const Value& v = sym->get();
			
			ASSERT( v.type() == V_desc );
			
			const Value::symdesc desc = v.desc();
			
			index = (short) desc;
			
			ASSERT( desc - index == 0 );
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
		
		expr = container.expr();
		
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
		
		if ( result.type() != V_bool )
		{
			throw assertion_result_not_boolean( expr->source );
		}
		
		if ( ! result.boolean() )
		{
			plus::var_string s = "assertion failed: ";
			
			s += rep( test );
			
			throw language_error( s, expr->source );
		}
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
	Value resolve_symbol_expr( const Value& v, const Value& stack )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_denote )
			{
				const Value& left = resolve_symbol( expr->left, stack );
				
				return eval( left, Op_denote, expr->right, expr->source );
			}
			
			if ( expr->op == Op_move )
			{
				return execute( v, stack );
			}
			
			const bool decl = declares_symbols( expr->op );
			const bool exec = ! decl  &&  expr->op != Op_list;
			
			return Value( decl ? expr->left
			                   : resolve_symbol_expr( expr->left, stack ),
			              expr->op,
			              exec ? execute            ( expr->right, stack )
			                   : resolve_symbol_expr( expr->right, stack ) );
		}
		
		if ( v.type() != Value_symbol )
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
		
		if ( expr == 0  ||  expr->op != Op_invocation )  // NULL
		{
			THROW( "`def` requires a block" );
		}
		
		Value lambda( Op_lambda, body );
		
		optimize_lambda_body( lambda.expr()->right );
		
		eval( name, Op_duplicate, lambda, expr->source );
		
		return name;
	}
	
	static const proc_info proc_define = { "define", &v_define, 0 };  // NULL
	
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
	
	Value execute( const Value& tree, const Value& stack )
	{
		if ( Expr* expr = tree.expr() )
		{
			if ( expr->op == Op_for )
			{
				run_for_loop( expr->right, stack );
				
				return Value();
			}
			
			if ( declares_symbols( expr->op ) )
			{
				if ( expr->right.type() != Value_symbol )
				{
					THROW( "declarator operand must be a symbol" );
				}
				
				if ( expr->op == Op_def )
				{
					const Value resolved = resolve_symbol( expr->right, stack );
					
					return bind_args( proc_define, resolved );
				}
				
				return Value_nothing;
			}
			
			if ( expr->op == Op_end )
			{
				execute( expr->left, stack );
				
				return execute( expr->right, stack );
			}
			
			if ( expr->op == Op_block )
			{
				const Value& scope = expr->right;
				
				return make_block_invocation( scope, stack );
			}
			
			if ( expr->op == Op_do_2 )
			{
				const Value test = invocable_expression( expr->left, stack );
				
				return Value( test, Op_do_2, execute( expr->right, stack ) );
			}
			
			if ( expr->op == Op_while_2 )
			{
				const Value test = invocable_expression( expr->right, stack );
				
				return Value( execute( expr->left, stack ), Op_while_2, test );
			}
			
			if ( is_elseif( expr ) )
			{
				const Value proc = invocable_expression( expr->right, stack );
				
				return Value( execute( expr->left, stack ), Op_else, proc );
			}
			
			if ( expr->op == Op_assert )
			{
				test_assertion( expr, stack );
				
				return Value();
			}
			
			if ( expr->op == Op_and  ||  expr->op == Op_or )
			{
				bool bail_on_truth = expr->op == Op_or;
				
				return short_circuit_logic( expr, bail_on_truth, stack );
			}
			
			const Value* left  = &expr->left;
			const Value* right = &expr->right;
			
			if ( left->type() == Value_dummy_operand )
			{
				using iota::swap;
				
				swap( left, right );
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
				if ( expr->op == Op_denote  &&  left->expr()->op == Op_def )
				{
					THROW( "function prototypes are unimplemented" );
				}
				
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
		
		const Value& resolved = resolve_symbol( tree, stack );
		
		return eval( resolved );
	}
	
	Value execute( const Value& root )
	{
		Expr* expr = root.expr();
		
		const Value stack( Value_nothing, Op_frame, expr->left );
		
		try
		{
			return execute( expr->right, stack );
		}
		catch ( const exception& e )
		{
			throw user_exception( e.message, expr->source );
		}
		
		// Silence Metrowerks C++ warning
		return Value();
	}
	
}
