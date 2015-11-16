/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	static
	Value assign_to_boolean( const Value& v )
	{
		if ( v.type() == Value_boolean )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_function( const Value& v )
	{
		if ( is_function( v ) )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_integer( const Value& v )
	{
		if ( v.type() == Value_number )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_string( const Value& v )
	{
		if ( v.type() == Value_string )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	static
	Value assign_to_type( const Value& v )
	{
		if ( v.type() == Value_base_type )
		{
			return v;
		}
		
		return Value_nothing;
	}
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type, &assign_to_##type }
	
	DEFINE_TYPE_INFO( boolean  );
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( integer  );
	DEFINE_TYPE_INFO( string   );
	DEFINE_TYPE_INFO( type     );
	
}
