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
	
	string unhex( const char* data, unsigned long size, align_t align )
	{
		// Alignment must be a power of two.
		
		ASSERT( align != 0 );
		
		const unsigned mask = align - 1;
		
		ASSERT( (align ^ mask) == (align + mask) );
		
		string::size_type n_data_bytes = size / 2 + (size & 1);
		string::size_type n_zero_bytes = mask - (n_data_bytes + mask & mask);
		string::size_type n_bytes      = n_data_bytes + n_zero_bytes;
		
		string result;
		
		char* p = result.reset( n_bytes );
		
		while ( n_zero_bytes-- )
		{
			*p++ = 0;
		}
		
		string::size_type n = n_data_bytes;
		
		if ( const bool has_unpaired_digit = size & 1 )
		{
			*p++ = gear::decoded_hex_digit( *data++ );
			--n;
		}
		
		for ( ;  n > 0;  --n )
		{
			*p++ = gear::decode_8_bit_hex( data );
			data += 2;
		}
		
		return result;
	}
	
}
