/*
	decimal.hh
	----------
*/

#ifndef PLUS_DECIMAL_HH
#define PLUS_DECIMAL_HH

// iota
#include "iota/string_traits.hh"

// plus
#include "plus/integer.hh"
#include "plus/string.hh"


namespace plus
{
	
	integer decode_decimal( const char* p, unsigned n );
	
	template < class String >
	inline
	integer decode_decimal( const String& s )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		return decode_decimal( get_string_data( s ), get_string_size( s ) );
	}
	
	
	string encode_decimal( const integer& x );
	
	inline
	string printable( const integer& x )
	{
		return encode_decimal( x );
	}
	
}

#endif
