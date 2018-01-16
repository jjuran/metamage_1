/*
	range_iterator.cc
	-----------------
*/

#include "vlib/iterators/range_iterator.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	static
	Value make_int_value_of_same_type( const bignum::integer& i, const Value& v )
	{
		if ( v.type() == Value_byte )
		{
			return Byte( i.clipped() );
		}
		
		return Integer( i );
	}
	
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
		its_value = make_int_value_of_same_type( its_next, its_value );
		
		return its_value;
	}
	
}
