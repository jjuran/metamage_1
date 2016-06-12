/*
	null.cc
	-------
*/

#include "vlib/types/null.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	Value Null::coerce( const Value& v )
	{
		if ( test( v ) )
		{
			return v;
		}
		
		switch ( v.type() )
		{
			default:
				THROW( "null conversion not defined for type" );
			
			case Value_empty_list:
				return Null();
		}
	}
	
	const type_info null_vtype =
	{
		"null",
		&assign_to< Null >,
		&Null::coerce,
		0,
	};
	
}
