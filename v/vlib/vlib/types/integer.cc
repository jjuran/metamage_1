/*
	integer.cc
	----------
*/

#include "vlib/types/integer.hh"

// iota
#include "iota/char_types.hh"

// plus
#include "plus/decimal.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/stringify.hh"


namespace vlib
{
	
	static
	bool is_decimal( const char* p, plus::string::size_type n )
	{
		if ( n == 0 )
		{
			return false;
		}
		
		if ( *p == '-'  ||  *p == '+' )
		{
			++p;
			
			if ( --n == 0 )
			{
				return false;
			}
		}
		
		do
		{
			if ( ! iota::is_digit( *p++ ) )
			{
				return false;
			}
		}
		while ( --n );
		
		return true;
	}
	
	static
	plus::integer decode_int( const plus::string& s )
	{
		if ( ! is_decimal( s.data(), s.size() ) )
		{
			THROW( "not a decimal integer" );
		}
		
		return plus::decode_decimal( s );
	}
	
	Value Integer::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_number:
				return v;
			
			default:
				THROW( "integer conversion not defined for type" );
			
			case Value_empty_list:
				return Integer();
			
			case Value_boolean:
				return Integer( v.boolean() + 0 );
			
			case Value_byte:
				return Integer( v.number() );
			
			case Value_string:
				return Integer( decode_int( v.string() ) );
		}
	}
	
	static
	size_t integer_str_size( const Value& v )
	{
		return decimal_length( v.number() );
	}
	
	static
	char* integer_str_copy( char* p, const Value& v )
	{
		return encode_decimal( p, v.number() );
	}
	
	static const stringify integer_str =
	{
		NULL,
		&integer_str_size,
		&integer_str_copy,
	};
	
	static const stringifiers integer_stringifiers =
	{
		&integer_str,
		// For rep, fall back to str
		// Integer bin is unimplemented
	};
	
	const dispatch integer_dispatch =
	{
		&integer_stringifiers,
	};
	
	const type_info integer_vtype =
	{
		"integer",
		&assign_to< Integer >,
		&Integer::coerce,
		0,
	};
	
}
