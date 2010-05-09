/*
	plus/hexidecimal.hh
	-------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef PLUS_HEXIDECIMAL_HH
#define PLUS_HEXIDECIMAL_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	class var_string;
	
	unsigned decode_32_bit_hex( const string& s );
	
	void encode_16_bit_hex( var_string& out, unsigned short x );
	
	void encode_32_bit_hex( var_string& out, unsigned x );
	
	string encode_16_bit_hex( unsigned short x );
	
	string encode_32_bit_hex( unsigned x );
	
}

#endif

