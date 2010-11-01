/*	===================
 *	iota/hexidecimal.cc
 *	===================
 */

#include "iota/hexidecimal.hh"


namespace iota
{
	
	static const unsigned char bad = 0;
	
	// (hex-digit & 0x1f) -> numeric nibble
	unsigned char decoded_hex_table[] =
	{
		bad, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, bad,
		bad, bad, bad, bad, bad, bad, bad, bad,
		0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
		0x8, 0x9, bad, bad, bad, bad, bad, bad
	};
	
	// (nibble & 0x0f) -> ASCII hex digit
	char encoded_hex_table[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	
	
	unsigned char decode_8_bit_hex( const char* s )
	{
		const unsigned x = decoded_hex_digit( s[ 0 ] ) <<  4
		                 | decoded_hex_digit( s[ 1 ] ) <<  0;
		
		return x;
	}
	
	unsigned short decode_16_bit_hex( const char* s )
	{
		const unsigned x = decoded_hex_digit( s[ 0 ] ) << 12
		                 | decoded_hex_digit( s[ 1 ] ) <<  8
		                 | decoded_hex_digit( s[ 2 ] ) <<  4
		                 | decoded_hex_digit( s[ 3 ] ) <<  0;
		
		return x;
	}
	
	unsigned decode_32_bit_hex( const char* s )
	{
		const unsigned x = decoded_hex_digit( s[ 0 ] ) << 28
		                 | decoded_hex_digit( s[ 1 ] ) << 24
		                 | decoded_hex_digit( s[ 2 ] ) << 20
		                 | decoded_hex_digit( s[ 3 ] ) << 16
		                 | decoded_hex_digit( s[ 4 ] ) << 12
		                 | decoded_hex_digit( s[ 5 ] ) <<  8
		                 | decoded_hex_digit( s[ 6 ] ) <<  4
		                 | decoded_hex_digit( s[ 7 ] ) <<  0;
		
		return x;
	}
	
	void encode_8_bit_hex( unsigned char x, char* s )
	{
		s[ 0 ] = encoded_hex_char( x >>  4 );
		s[ 1 ] = encoded_hex_char( x >>  0 );
	}
	
	void encode_16_bit_hex( unsigned short x, char* s )
	{
		s[ 0 ] = encoded_hex_char( x >> 12 );
		s[ 1 ] = encoded_hex_char( x >>  8 );
		s[ 2 ] = encoded_hex_char( x >>  4 );
		s[ 3 ] = encoded_hex_char( x >>  0 );
	}
	
	void encode_32_bit_hex( unsigned x, char* s )
	{
		s[ 0 ] = encoded_hex_char( x >> 28 );
		s[ 1 ] = encoded_hex_char( x >> 24 );
		s[ 2 ] = encoded_hex_char( x >> 20 );
		s[ 3 ] = encoded_hex_char( x >> 16 );
		s[ 4 ] = encoded_hex_char( x >> 12 );
		s[ 5 ] = encoded_hex_char( x >>  8 );
		s[ 6 ] = encoded_hex_char( x >>  4 );
		s[ 7 ] = encoded_hex_char( x >>  0 );
	}
	
	void inscribe_n_hex_digits( char* p, unsigned long x, unsigned short n )
	{
		switch ( n )
		{
			default:
				while ( --n >= 8 )
				{
					*p++ = '0';
				}
				
				// fall through
			
			case 8:
				*p++ = encoded_hex_char( x >> 28 );
				// fall through
			
			case 7:
				*p++ = encoded_hex_char( x >> 24 );
				// fall through
			
			case 6:
				*p++ = encoded_hex_char( x >> 20 );
				// fall through
			
			case 5:
				*p++ = encoded_hex_char( x >> 16 );
				// fall through
			
			case 4:
				*p++ = encoded_hex_char( x >> 12 );
				// fall through
			
			case 3:
				*p++ = encoded_hex_char( x >>  8 );
				// fall through
			
			case 2:
				*p++ = encoded_hex_char( x >>  4 );
				// fall through
			
			case 1:
				*p   = encoded_hex_char( x       );
				// fall through
			
			case 0:
				break;
		}
	}
	
}

