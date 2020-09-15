/*
	decode_binoid_int.hh
	--------------------
*/

#ifndef BIGNUM_DECODEBINOIDINT_HH
#define BIGNUM_DECODEBINOIDINT_HH

// iota
#include "iota/string_traits.hh"

// bignum
#include "bignum/integer.hh"


namespace bignum
{
	
	integer unbin_int( const char* p, unsigned n );
	integer unhex_int( const char* p, unsigned n );
	
	template < class String >
	inline
	integer unbin_int( const String& s )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		return unbin_int( get_string_data( s ), get_string_size( s ) );
	}
	
	template < class String >
	inline
	integer unhex_int( const String& s )
	{
		using iota::get_string_data;
		using iota::get_string_size;
		
		return unhex_int( get_string_data( s ), get_string_size( s ) );
	}
	
}

#endif
