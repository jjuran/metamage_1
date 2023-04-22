/*
	plus/hexadecimal.cc
	-------------------
*/

#include "plus/hexadecimal.hh"

// gear
#include "gear/hexadecimal.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"


namespace plus
{
	
	static const unsigned short n_bits_per_nibble = 4;
	
	
	unsigned decode_32_bit_hex( const string& s )
	{
		const unsigned short n_nibbles = 32 / n_bits_per_nibble;
		
		if ( s.length() < n_nibbles )
		{
			return 0;
		}
		
		return gear::decode_32_bit_hex( s.data() );
	}
	
	void encode_8_bit_hex( var_string& out, unsigned char x )
	{
		const unsigned short n_nibbles = 8 / n_bits_per_nibble;
		
		char buffer[ n_nibbles ];
		
		gear::encode_8_bit_hex( x, buffer );
		
		out.append( buffer, sizeof buffer );
	}
	
	void encode_16_bit_hex( var_string& out, unsigned short x )
	{
		const unsigned short n_nibbles = 16 / n_bits_per_nibble;
		
		char buffer[ n_nibbles ];
		
		gear::encode_16_bit_hex( x, buffer );
		
		out.append( buffer, sizeof buffer );
	}
	
	void encode_32_bit_hex( var_string& out, unsigned x )
	{
		const unsigned short n_nibbles = 32 / n_bits_per_nibble;
		
		char buffer[ n_nibbles ];
		
		gear::encode_32_bit_hex( x, buffer );
		
		out.append( buffer, sizeof buffer );
	}
	
	string encode_8_bit_hex( unsigned char x )
	{
		var_string result;
		
		encode_8_bit_hex( result, x );
		
		return result;
	}
	
	string encode_16_bit_hex( unsigned short x )
	{
		var_string result;
		
		encode_16_bit_hex( result, x );
		
		return result;
	}
	
	string encode_32_bit_hex( unsigned x )
	{
		var_string result;
		
		encode_32_bit_hex( result, x );
		
		return result;
	}
	
	string hex_lower( const void* in, unsigned long n )
	{
		string result;
		
		ASSERT( n <= (unsigned long) -1 / 2 );
		
		char* p = result.reset( n * 2 );
		
		gear::hexpcpy_lower( p, in, n );
		
		return result;
	}
	
	string hex_upper( const void* in, unsigned long n )
	{
		string result;
		
		ASSERT( n <= (unsigned long) -1 / 2 );
		
		char* p = result.reset( n * 2 );
		
		gear::hexpcpy_upper( p, in, n );
		
		return result;
	}
	
	
}
