/*
	canonical_32_bit_hex.cc
	-----------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/canonical_32_bit_hex.hh"

// Standard C
#include <ctype.h>

// plus
#include "plus/contains.hh"


namespace Genie
{
	
	struct small_hex_digit
	{
		static bool applies( char c )
		{
			// hexidecimal digits are one of:
			// * ['0' - '9']  0x30 - 0x39
			// * ['A' - 'Z']  0x41 - 0x5a
			// * ['a' - 'z']  0x61 - 0x7a
			
			return c & 0x20  &&  isxdigit( c );  // accept only 0-9, a-z
		}
		
		bool operator()( char c ) const
		{
			return applies( c );
		}
	};
	
	struct no_small_hex_digit
	{
		bool operator()( char c ) const
		{
			return !small_hex_digit::applies( c );
		}
	};
	
	struct only_small_hexes
	{
		static bool applies( const std::string& s )
		{
			return !plus::contains_if( s, no_small_hex_digit() );
		}
	};
	
	bool canonical_32_bit_hex::applies( const std::string& name )
	{
		const unsigned bits_per_byte      =  8;
		const unsigned hex_chars_per_byte =  2;
		
		const unsigned n_bits             = 32;
		
		const unsigned n_hexes = n_bits / bits_per_byte * hex_chars_per_byte;
		
		return name.size() == n_hexes  &&  only_small_hexes::applies( name );
	}
	
}

