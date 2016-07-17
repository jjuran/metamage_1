/*
	boolean.cc
	----------
*/

#include "vlib/types/boolean.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/type_info.hh"
#include "vlib/types/null.hh"


namespace vlib
{
	
	Value Boolean::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_boolean:
				return v;
			
			default:
				INTERNAL_ERROR( "invalid type in coerce_to_boolean()" );
			
			case Value_empty_list:
			case Value_empty_array:
				return Bool();
			
			case Value_byte:
			case Value_number:
				return Bool( ! v.number().is_zero() );
			
			case Value_data:
			case Value_string:
				return Bool( ! v.string().empty() );
			
			case Value_base_type:
				return Bool( &v.typeinfo() != &null_vtype );
			
			case Value_function:
			case Value_pair:
				return True;
		}
	}
	
	const type_info boolean_vtype =
	{
		"boolean",
		&assign_to< Boolean >,
		&Boolean::coerce,
		0,
	};
	
}
