/*
	stack.cc
	--------
*/

#include "vlib/stack.hh"

// debug
#include "debug/assert.hh"


namespace vlib
{
	
	const Value& nth_frame( const Value& stack, unsigned depth )
	{
		const Value* frame = &stack;
		
		while ( depth-- != 0 )
		{
			Expr* expr = frame->expr();
			
			ASSERT( expr != NULL );
			ASSERT( expr->op == Op_frame );
			
			frame = &expr->left;
		}
		
		return *frame;
	}
	
}
