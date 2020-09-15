/*
	peephole.cc
	-----------
*/

#include "vlib/peephole.hh"

// debug
#include "debug/assert.hh"


namespace vlib
{
	
	/*
		def f
		{
			...
			
			return result;
		}
		
		|
		|
		V
		
		invocation
			activation
					first statement
				`;`
						...
					`;`
							return
								result
						`;`
							(empty statement)
		
		[left operands of invocation and activation omitted for clarity]
		
		The problem:
		
		It's much faster to fall off the end of a function than to return
		from it.  However, manually eliding the `return` operator and dropping
		the final semicolon were poor style in multiline functions even before
		the syntax changed to allow newlines to terminate statements, and now
		that that's happened, it would be necessary to remove any newlines
		between the value and the closing right brace, which stylistically is
		simply indefensible.
		
		[Update:  Newlines can be escaped with `\`, and a trailing comma has
		the same effect before a right brace, but it's not acceptable to have
		a huge performance boost controlled by an undiscoverable syntax hack.]
		
		The solution:
		
		Always write `return` in multiline functions, and elide the operator
		and subsequent empty statement in an optimization pass.
		
		Notes:
		
		We don't actually verify that the last statement is empty, because it
		doesn't matter. `lambda {...; return 123; 456}` still returns 123.
		
		This only applies to lambdas.  A `return` in a non-lambda block still
		needs to throw an exception.
	*/
	
	static
	bool has_top_level_return( Expr* expr )
	{
	tail_call:
		
		if ( expr->op == Op_end )
		{
			Expr* left_expr = expr->left.expr();
			
			if ( left_expr  &&  left_expr->op == Op_return )
			{
				return true;
			}
			
			if (( expr = expr->right.expr() ))
			{
				goto tail_call;
			}
			
			return false;
		}
		
		return expr->op == Op_return;
	}
	
	void optimize_lambda_body( Value& body )
	{
		Expr* expr = body.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_invocation );
		
		expr = expr->right.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_activation );
		
		if ( ! (expr = expr->right.expr())  ||  ! has_top_level_return( expr ) )
		{
			return;
		}
		
		// This is the block's root expression.
		Value& root = body.unshare().expr()->right.unshare().expr()->right;
		
		if ( expr->op == Op_return )
		{
			// Elide the `return` operator.
			root = expr->right;
			return;
		}
		
		Value* it = &root;
		
		expr = it->expr();
		
		do
		{
			Expr* left_expr = expr->left.expr();
			
			if ( left_expr->op == Op_return )
			{
				// Elide the `return` operator and subsequent statements.
				*it = left_expr->right;
				return;
			}
			
			it = &it->unshare().expr()->right;
			
			expr = it->expr();
			
			if ( expr  &&  expr->op == Op_return )
			{
				// Elide the `return` operator.
				*it = expr->right;
				return;
			}
		}
		while ( expr  &&  expr->op == Op_end );
	}
	
}
