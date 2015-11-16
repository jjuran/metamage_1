/*
	functions.cc
	------------
*/

#include "vlib/functions.hh"

// plus
#include "plus/binary.hh"
#include "plus/decode_binoid_int.hh"
#include "plus/hexadecimal.hh"
#include "plus/integer_hex.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"


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
		return make_string( value, Stringified_to_print );
	}
	
	static
	Value v_bool( const Value& arg )
	{
		switch ( get_type( arg ) )
		{
			default:
				INTERNAL_ERROR( "invalid type in v_bool()" );
			
			case Value_empty_list:
				return false;
			
			case Value_boolean:
				return arg;
			
			case Value_number:
				return ! get_int( arg ).is_zero();
			
			case Value_string:
				return ! get_str( arg ).empty();
			
			case Value_function:
			case Value_pair:
				return true;
		}
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( get_type( arg ) )
		{
			default:  TYPE_ERROR( "invalid argument to hex()" );
			
			case Value_number:  return hex( get_int( arg ) );
			case Value_string:  return hex( get_str( arg ) );
		}
	}
	
	static
	Value v_abs( const Value& arg )
	{
		if ( get_type( arg ) != Value_number )
		{
			TYPE_ERROR( "invalid argument to abs()" );
		}
		
		return abs( get_int( arg ) );
	}
	
	static
	Value v_half( const Value& arg )
	{
		if ( get_type( arg ) != Value_number )
		{
			TYPE_ERROR( "invalid argument to half()" );
		}
		
		return half( get_int( arg ) );
	}
	
	static
	Value v_area( const Value& v )
	{
		return area( v );
	}
	
	static
	Value v_rep( const Value& v )
	{
		return make_string( v, Stringified_to_reproduce );
	}
	
	static
	bool is_0x_numeral( const plus::string& s, char x )
	{
		return s.size() > 2  &&  s[ 0 ] == '0'  &&  s[ 1 ] == x;
	}
	
	static
	Value v_unbin( const Value& v )
	{
		if ( get_type( v ) != Value_string )
		{
			TYPE_ERROR( "unbin() argument must be a string" );
		}
		
		if ( is_0x_numeral( get_str( v ), 'b' ) )
		{
			return unbin_int( get_str( v ).substr( 2 ) );
		}
		
		return unbin( get_str( v ) );
	}
	
	static
	Value v_unhex( const Value& v )
	{
		if ( get_type( v ) != Value_string )
		{
			TYPE_ERROR( "unhex() argument must be a string" );
		}
		
		if ( is_0x_numeral( get_str( v ), 'x' ) )
		{
			return unhex_int( get_str( v ).substr( 2 ) );
		}
		
		return unhex( get_str( v ) );
	}
	
	const proc_info proc_abs   = { &v_abs,   "abs"   };
	const proc_info proc_area  = { &v_area,  "area"  };
	const proc_info proc_bool  = { &v_bool,  "bool"  };
	const proc_info proc_half  = { &v_half,  "half"  };
	const proc_info proc_hex   = { &v_hex,   "hex"   };
	const proc_info proc_rep   = { &v_rep,   "rep"   };
	const proc_info proc_str   = { &v_str,   "str"   };
	const proc_info proc_unbin = { &v_unbin, "unbin" };
	const proc_info proc_unhex = { &v_unhex, "unhex" };
	
}
