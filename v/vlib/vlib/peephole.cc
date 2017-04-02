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
		
		The solution:
		
		Always write `return` in multiline functions, and elide the operator
		and subsequent empty statement in an optimization pass.
		
		Notes:
		
		We don't actually verify that the last statement is empty, because it
		doesn't matter. `lambda {...; return 123; 456}` still returns 123.
		
		This only applies to lambdas.  A `return` in a non-lambda block still
		needs to throw an exception.
		
		TODO:
		
			lambda { ...; return 123  }  # `return` is the final statement
			lambda {      return 123; }  # `return` is the first statement
	*/
	
	void optimize_lambda_body( Value& body )
	{
		Expr* expr = body.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_invocation );
		
		expr = expr->right.expr();
		
		ASSERT( expr != 0 );  // NULL
		ASSERT( expr->op == Op_activation );
		
		if ( !(expr = expr->right.expr())  ||  expr->op != Op_end    )  return;
		if ( !(expr = expr->left .expr())  ||  expr->op != Op_end    )  return;
		if ( !(expr = expr->right.expr())  ||  expr->op != Op_return )  return;
		
		// This is the block's root expression.
		Value& root = body.unshare().expr()->right.unshare().expr()->right;
		
		// Discard the last statement.
		root = root.expr()->left;
		
		// This is the `return` operation.
		Value& end = root.unshare().expr()->right;
		
		// Elide the `return` operator.
		end = end.expr()->right;
	}
	
}
