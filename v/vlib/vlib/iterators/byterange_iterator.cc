/*
	byterange_iterator.cc
	---------------------
*/

#include "vlib/iterators/byterange_iterator.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types/byte.hh"


namespace vlib
{
	
	byterange_iterator::byterange_iterator( const Value& range )
	{
		if ( Expr* expr = range.expr() )
		{
			const op_type op = expr->op;
			
			if ( op == Op_gamut )
			{
				its_next = expr->left .to< Byte >();
				its_high = expr->right.to< Byte >();
				
				its_value = expr->left;
				
				return;
			}
		}
		
		THROW( "invalid range object" );
	}
	
	const Value& byterange_iterator::get() const
	{
		its_value = Byte( its_next );
		
		return its_value;
	}
	
}
