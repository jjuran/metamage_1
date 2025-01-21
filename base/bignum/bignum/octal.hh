/*
	octal.hh
	--------
*/

#ifndef BIGNUM_OCTAL_HH
#define BIGNUM_OCTAL_HH

// iota
#include "iota/string_traits.hh"

// bignum
#include "bignum/integer.hh"


namespace bignum
{
	
	integer decode_octal( const char* p, unsigned n );
	
	template < class String >
	inline
	integer decode_octal( const String& s )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		return decode_octal( get_string_data( s ), get_string_size( s ) );
	}
	
}

#endif
