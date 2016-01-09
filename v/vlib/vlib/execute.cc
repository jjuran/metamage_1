/*
	execute.cc
	----------
*/

#include "vlib/execute.hh"

// vlib
#include "vlib/eval.hh"
#include "vlib/proc_info.hh"
#include "vlib/symbol_table.hh"


namespace vlib
{
	
	static
	Value execute( const Value& tree, const Value& stack );
	
	static
	Value v_invoke( const Value& v )
	{
		Expr* expr = v.expr();
		
		if ( expr != NULL  &&  expr->op == Op_bind_args )
		{
			const Value& block = expr->left;
			const Value& args  = expr->right;
			
			Symbol* const underscore = locate_symbol( "_" );
			
			const Value previous = underscore->get();
			
			underscore->assign( args );
			
			const Value result = execute( block, args );
			
			underscore->assign( previous );
			
			return result;
		}
		
		return execute( v, empty_list );
	}
	
	static proc_info proc_invoke = { &v_invoke, "invoke", 0 };
	
	Value execute( const Value& tree, const Value& stack )
	{
		if ( Expr* expr = tree.expr() )
		{
			if ( expr->op == Op_var  ||  expr->op == Op_const )
			{
				return Value_nothing;
			}
			
			if ( expr->op == Op_end )
			{
				execute( expr->left, stack );
				
				return execute( expr->right, stack );
			}
			
			if ( expr->op == Op_block )
			{
				return Value( proc_invoke, Op_invocation, expr->right );
			}
			
			if ( expr->op == Op_do )
			{
				const Value test( proc_invoke, Op_invocation, expr->left );
				
				return Value( test, Op_do, execute( expr->right, stack ) );
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
				return eval( *left,
				             expr->op,
				             execute( *right, stack ) );
			}
			
			/*
				WARNING:  This recursion is unsafe.  It can be abused with a
				sufficiently long program, and recursive user-defined blocks
				only make this easier.
			*/
			
			return eval( execute( *left, stack ),
			             expr->op,
			             execute( *right, stack ) );
		}
		
		return eval( tree );
	}
	
	Value execute( const Value& root )
	{
		return execute( root, empty_list );
	}
	
}
