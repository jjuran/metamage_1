/*
	functions.cc
	------------
*/

#include "vlib/functions.hh"

// plus
#include "plus/string/mince.hh"

// plus
#include "plus/binary.hh"
#include "plus/decode_binoid_int.hh"
#include "plus/hexadecimal.hh"
#include "plus/integer_hex.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/types.hh"


namespace vlib
{
	
	static
	plus::string hex( const plus::string& s )
	{
		return plus::hex_lower( s.data(), s.size() );
	}
	
	static
	Value v_hex( const Value& arg )
	{
		switch ( arg.type() )
		{
			default:  TYPE_ERROR( "invalid argument to hex()" );
			
			case Value_number:  return hex( get_int( arg ) );
			case Value_string:  return hex( get_str( arg ) );
		}
	}
	
	static
	Value v_abs( const Value& arg )
	{
		return abs( get_int( arg ) );
	}
	
	static
	Value v_half( const Value& arg )
	{
		return half( get_int( arg ) );
	}
	
	static
	Value v_area( const Value& v )
	{
		return area( v );
	}
	
	static
	Value v_mince( const Value& v )
	{
		typedef plus::string::size_type size_t;
		
		const plus::string& string = first( v ).string();
		const size_t        stride = rest ( v ).number().clipped();
		
		if ( stride == 0 )
		{
			DOMAIN_ERROR( "mince() stride must be positive" );
		}
		
		return mince( string, stride );
	}
	
	static
	Value v_rep( const Value& v )
	{
		return make_string( v, Stringified_to_reproduce );
	}
	
	static
	plus::string::size_type substr_offset( const plus::string&   s,
	                                       const plus::integer&  offset )
	{
		if ( offset > s.size() )
		{
			DOMAIN_ERROR( "substr offset exceeds string length" );
		}
		
		return offset.clipped();
	}
	
	static inline
	plus::string::size_type substr_length( const plus::string&   s,
	                                       const plus::integer&  length )
	{
		return length.clipped();
	}
	
	static
	Value v_substr( const Value& v )
	{
		const Value& arg1 = first( v );
		const Value& arg1_ = rest( v );
		const Value& arg2 = first( arg1_ );
		const Value& arg2_ = rest( arg1_ );
		const Value& arg3 = first( arg2_ );
		
		typedef plus::string::size_type size_t;
		
		const plus::string& s = arg1.string();
		
		const size_t offset = substr_offset( s, arg2.number() );
		const size_t length = substr_length( s, arg3.number() );
		
		return s.substr( offset, length );
	}
	
	static
	bool is_0x_numeral( const plus::string& s, char x )
	{
		return s.size() > 2  &&  s[ 0 ] == '0'  &&  s[ 1 ] == x;
	}
	
	static
	Value v_unbin( const Value& v )
	{
		if ( is_0x_numeral( get_str( v ), 'b' ) )
		{
			return unbin_int( get_str( v ).substr( 2 ) );
		}
		
		return unbin( get_str( v ) );
	}
	
	static
	Value v_unhex( const Value& v )
	{
		if ( is_0x_numeral( get_str( v ), 'x' ) )
		{
			return unhex_int( get_str( v ).substr( 2 ) );
		}
		
		return unhex( get_str( v ) );
	}
	
	static const Value integer = integer_vtype;
	static const Value string  = string_vtype;
	
	static const Value u32_2 = Value( u32_vtype, Op_duplicate, 2 );
	static const Value mince = Value( string, u32_2 );
	
	static const Value s_offset( u32_vtype, Op_duplicate, 0 );
	static const Value s_length( u32_vtype, Op_duplicate, uint32_t( -1 ) );
	static const Value substr( string_vtype, Value( s_offset, s_length ) );
	
	const proc_info proc_abs    = { &v_abs,    "abs",    &integer };
	const proc_info proc_area   = { &v_area,   "area",   NULL     };
	const proc_info proc_half   = { &v_half,   "half",   &integer };
	const proc_info proc_hex    = { &v_hex,    "hex",    NULL     };
	const proc_info proc_mince  = { &v_mince,  "mince",  &mince   };
	const proc_info proc_rep    = { &v_rep,    "rep",    NULL     };
	const proc_info proc_substr = { &v_substr, "substr", &substr  };
	const proc_info proc_unbin  = { &v_unbin,  "unbin",  &string  };
	const proc_info proc_unhex  = { &v_unhex,  "unhex",  &string  };
	
}
