/*
	functions.cc
	------------
*/

#include "vlib/functions.hh"

// plus
#include "plus/hexadecimal.hh"
#include "plus/integer_hex.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/string-utils.hh"


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vlib
{
	
	static
	plus::string hex( const plus::string& s )
	{
		return plus::hex_lower( s.data(), s.size() );
	}
	
	static
	Value v_str( const Value& value )
	{
		return make_string( value );
	}
	
	static
	Value v_bool( const Value& arg )
	{
		switch ( arg.type )
		{
			default:
				INTERNAL_ERROR( "invalid type in v_bool()" );
			
			case Value_empty_list:
				return false;
			
			case Value_boolean:
				return arg;
			
			case Value_number:
				return ! arg.number.is_zero();
			
			case Value_string:
				return ! arg.string.empty();
			
			case Value_function:
			case Value_pair:
				return true;
		}
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( arg.type )
		{
			default:  TYPE_ERROR( "invalid argument to hex()" );
			
			case Value_number:  return hex( arg.number );
			case Value_string:  return hex( arg.string );
		}
	}
	
	static
	Value v_abs( const Value& arg )
	{
		if ( arg.type != Value_number )
		{
			TYPE_ERROR( "invalid argument to abs()" );
		}
		
		return abs( arg.number );
	}
	
	static
	Value v_half( const Value& arg )
	{
		if ( arg.type != Value_number )
		{
			TYPE_ERROR( "invalid argument to half()" );
		}
		
		return half( arg.number );
	}
	
	struct function_mapping
	{
		const char*    name;
		function_type  f;
	};
	
	static
	const function_mapping functions[] =
	{
		{ "abs",  &v_abs  },
		{ "bool", &v_bool },
		{ "half", &v_half },
		{ "hex",  &v_hex  },
		{ "str",  &v_str  },
	};
	
	static
	const function_mapping* find( const plus::string& name )
	{
		const function_mapping* begin = functions;
		const function_mapping* end   = ARRAY_END( functions );
		
		const function_mapping* it = begin;
		
		while ( it < end )
		{
			if ( it->name == name )
			{
				return it;
			}
			
			++it;
		}
		
		return 0;  // NULL
	}
	
	function_type function_from_name( const plus::string& name )
	{
		if ( const function_mapping* it = find( name ) )
		{
			return it->f;
		}
		
		return 0;  // NULL
	}
	
}
