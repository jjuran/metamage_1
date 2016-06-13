/*
	byte.cc
	-------
*/

#include "vlib/types/byte.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	Value Byte::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_byte:
				return v;
			
			default:
				THROW( "byte conversion not defined for type" );
			
			case Value_empty_list:
				return Byte();
			
			case Value_number:
				return Byte( v.number().clipped() * v.number().sign() );
		}
	}
	
	const type_info byte_vtype =
	{
		"byte",
		&assign_to< Byte >,
		&Byte::coerce,
		0,
	};
	
}
