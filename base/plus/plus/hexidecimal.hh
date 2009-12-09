/*
	plus/hexidecimal.hh
	-------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef PLUS_HEXIDECIMAL_HH
#define PLUS_HEXIDECIMAL_HH

// Standard C++
#include <string>


namespace plus
{
	
	unsigned decode_32_bit_hex( const std::string& s );
	
	void encode_16_bit_hex( unsigned short x, std::string& result );
	
	void encode_32_bit_hex( unsigned x, std::string& result );
	
	std::string encode_16_bit_hex( unsigned short x );
	
	std::string encode_32_bit_hex( unsigned x );
	
}

#endif

