/*
	data.cc
	-------
*/

#include "vlib/types/data.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/types/integer.hh"


namespace vlib
{
	
	Value Data::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_data:
				return v;
			
			default:
				THROW( "type not convertible to data" );
			
			case Value_empty_list:
				return Data();
			
			case Value_string:
				return Data( v.string() );
			
			case Value_byte:
			case Value_pair:
				return pack( v );
		}
	}
	
	static
	Value data_member( const Value&         obj,
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
		
		THROW( "nonexistent data member" );
		
		return Value_nothing;
	}
	
	const type_info data_vtype =
	{
		"data",
		&assign_to< Data >,
		&Data::coerce,
		&data_member,
	};
	
}
