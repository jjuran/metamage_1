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
#include "vlib/string-utils.hh"
#include "vlib/symbol_table.hh"


namespace vlib
{
	
	void define( const proc_info& proc )
	{
		const symbol_id sym = create_symbol( proc.name, Symbol_const );
		
		assign_symbol( sym, Value( proc.addr, proc.name ) );
	}
	
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
	
	static const proc_info proc_abs  = { &v_abs,  "abs"  };
	static const proc_info proc_bool = { &v_bool, "bool" };
	static const proc_info proc_half = { &v_half, "half" };
	static const proc_info proc_hex  = { &v_hex,  "hex"  };
	static const proc_info proc_str  = { &v_str,  "str"  };
	
	bool install_basic_functions()
	{
		define( proc_abs  );
		define( proc_bool );
		define( proc_half );
		define( proc_hex  );
		define( proc_str  );
		
		return true;
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
		if ( v.type != Value_string )
		{
			TYPE_ERROR( "unbin() argument must be a string" );
		}
		
		if ( is_0x_numeral( v.string, 'b' ) )
		{
			return unbin_int( v.string.substr( 2 ) );
		}
		
		return unbin( v.string );
	}
	
	static
	Value v_unhex( const Value& v )
	{
		if ( v.type != Value_string )
		{
			TYPE_ERROR( "unhex() argument must be a string" );
		}
		
		if ( is_0x_numeral( v.string, 'x' ) )
		{
			return unhex_int( v.string.substr( 2 ) );
		}
		
		return unhex( v.string );
	}
	
	const proc_info proc_area  = { &v_area,  "area"  };
	const proc_info proc_rep   = { &v_rep,   "rep"   };
	const proc_info proc_unbin = { &v_unbin, "unbin" };
	const proc_info proc_unhex = { &v_unhex, "unhex" };
	
}
