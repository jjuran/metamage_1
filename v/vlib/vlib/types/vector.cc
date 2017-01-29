/*
	vector.cc
	---------
*/

#include "vlib/types/vector.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	Value Vector::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_vector:
				return v;
			
			default:
				THROW( "type not convertible to vector" );
			
			case Value_empty_list:
				return Vector();
			
			case Value_string:
				return Vector( v.string() );
			
			case Value_byte:
			case Value_pair:
				return Vector( pack( v ) );
		}
	}
	
	static
	Value vector_member( const Value&         obj,
	                     const plus::string&  member )
	{
		if ( member == "size" )
		{
			return Integer( obj.string().size() );
		}
		
		if ( member == "string" )
		{
			return obj.string();
		}
		
		THROW( "nonexistent vector member" );
		
		return Value_nothing;
	}
	
	const type_info vector_vtype =
	{
		"vector",
		&assign_to< Vector >,
		&Vector::coerce,
		&vector_member,
	};
	
}
