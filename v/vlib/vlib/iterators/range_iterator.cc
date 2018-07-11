/*
	range_iterator.cc
	-----------------
*/

#include "vlib/iterators/range_iterator.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	range_iterator::range_iterator( const Value& range )
	{
		if ( Expr* expr = range.expr() )
		{
			const op_type op = expr->op;
			
			if ( op == Op_gamut  ||  op == Op_delta )
			{
				its_next = expr->left .number();
				its_high = expr->right.number() - (op == Op_delta);
				
				its_value = expr->left;
				
				return;
			}
		}
		
		THROW( "invalid range object" );
	}
	
	const Value& range_iterator::get() const
	{
		its_value = Integer( its_next );
		
		return its_value;
	}
	
}
