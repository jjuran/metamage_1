/*
	plus/hexidecimal.cc
	-------------------
	
	Copyright 2009, Joshua Juran
*/

#include "plus/hexidecimal.hh"

// iota
#include "iota/hexidecimal.hh"


namespace plus
{
	
	static const unsigned short n_bits_per_nibble = 4;
	
	
	unsigned decode_32_bit_hex( const std::string& s )
	{
		const unsigned short n_nibbles = 32 / n_bits_per_nibble;
		
		if ( s.length() < n_nibbles )
		{
			return 0;
		}
		
		return iota::decode_32_bit_hex( s.data() );
	}
	
	void encode_16_bit_hex( unsigned short x, std::string& result )
	{
		const unsigned short n_nibbles = 16 / n_bits_per_nibble;
		
		result.resize( n_nibbles );
		
		iota::encode_16_bit_hex( x, &result[0] );
	}
	
	void encode_32_bit_hex( unsigned x, std::string& result )
	{
		const unsigned short n_nibbles = 32 / n_bits_per_nibble;
		
		result.resize( n_nibbles );
		
		iota::encode_32_bit_hex( x, &result[0] );
	}
	
	std::string encode_16_bit_hex( unsigned short x )
	{
		std::string result;
		
		encode_16_bit_hex( x, result );
		
		return result;
	}
	
	std::string encode_32_bit_hex( unsigned x )
	{
		std::string result;
		
		encode_32_bit_hex( x, result );
		
		return result;
	}
	
}

