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
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/symdesc.hh"
#include "vlib/throw.hh"
#include "vlib/tracker.hh"
#include "vlib/type_info.hh"
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
		
		symbol.sym()->assign( v );
		
		return symbol;
	}
	
	static
	Value unshare_symbols( const Value& presets )
	{
		if ( is_empty( presets ) )
		{
			return Value_empty_list;
		}
		
		const Value& head = first( presets );
		const Value& tail = rest ( presets );
		
		const Symbol* sym = head.sym();
		
		const Value new_head = sym->is_immutable() ? head : sym->clone();
		
		if ( is_empty( tail ) )
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
	Value make_test_expression( const Value& tree, const Value& stack )
	{
		const Value expr( stack,       Op_expression, tree );
		const Value test( proc_invoke, Op_invocation, expr );
		
		return test;
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
		
		const bool truth = boolean_vtype.coerce( result ).boolean();
		
		if ( truth == bail )
		{
			return result;
		}
		
		return execute( expr->right, stack );
	}
	
	static
	Value resolve_symbol_list( const Value& v, const Value& stack )
	{
		if ( Expr* expr = v.expr() )
		{
			if ( expr->op == Op_denote )
			{
				const Value& left = resolve_symbol( expr->left, stack );
				
				return eval( left, Op_denote, expr->right, expr->source );
			}
			
			if ( expr->op == Op_list )
			{
				return Value( resolve_symbol_list( expr->left,  stack ),
				              resolve_symbol_list( expr->right, stack ) );
			}
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
		
		const Value lambda( Op_lambda, body );
		
		eval( name, Op_duplicate, lambda, expr->source );
		
		return name;
	}
	
	static const proc_info proc_define = { "define", &v_define, 0 };  // NULL
	
	Value execute( const Value& tree, const Value& stack )
	{
		if ( Expr* expr = tree.expr() )
		{
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
				const Value test = make_test_expression( expr->left, stack );
				
				return Value( test, Op_do_2, execute( expr->right, stack ) );
			}
			
			if ( expr->op == Op_while_2 )
			{
				const Value test = make_test_expression( expr->right, stack );
				
				return Value( execute( expr->left, stack ), Op_while_2, test );
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
			
			if ( is_left_varop( expr->op )  &&  ! is_type_annotation( *left ) )
			{
				if ( expr->op == Op_denote  &&  left->expr()->op == Op_def )
				{
					THROW( "function prototypes are unimplemented" );
				}
				
				return eval( resolve_symbol_list( *left, stack ),
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
