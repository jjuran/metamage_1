/*
	range.cc
	--------
*/

#include "vlib/types/range.hh"

// vlib
#include "vlib/throw.hh"


namespace vlib
{
	
	static
	void check_range_operands( const Value& left, const Value& right )
	{
		if ( left.type() != Value_number  &&  left.type() != Value_byte )
		{
			THROW( "non-integer range operand" );
		}
		
		if ( right.type() != left.type() )
		{
			THROW( "type-mismatched range operands" );
		}
	}
	
	Range::Range( const Value& left, op_type op, const Value& right )
	:
		Value( left, op, right, 0 )  // NULL
	{
		check_range_operands( left, right );
	}
	
}
