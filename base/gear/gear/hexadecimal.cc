/*
	gear/hexadecimal.cc
	-------------------
*/

#include "gear/hexadecimal.hh"


namespace gear
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
	
	// (nibble & 0x0f) -> ASCII hex digit
	char encoded_HEX_table[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
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
	
	char* encode_8_bit_hex( unsigned char x, char* p )
	{
		*p++ = encoded_hex_char( x >>  4 );
		*p++ = encoded_hex_char( x >>  0 );
		
		return p;
	}
	
	char* encode_8_bit_HEX( unsigned char x, char* p )
	{
		*p++ = encoded_HEX_char( x >>  4 );
		*p++ = encoded_HEX_char( x >>  0 );
		
		return p;
	}
	
	char* encode_16_bit_hex( unsigned short x, char* p )
	{
		*p++ = encoded_hex_char( x >> 12 );
		*p++ = encoded_hex_char( x >>  8 );
		*p++ = encoded_hex_char( x >>  4 );
		*p++ = encoded_hex_char( x >>  0 );
		
		return p;
	}
	
	char* encode_16_bit_HEX( unsigned short x, char* p )
	{
		*p++ = encoded_HEX_char( x >> 12 );
		*p++ = encoded_HEX_char( x >>  8 );
		*p++ = encoded_HEX_char( x >>  4 );
		*p++ = encoded_HEX_char( x >>  0 );
		
		return p;
	}
	
	char* encode_32_bit_hex( unsigned x, char* p )
	{
		*p++ = encoded_hex_char( x >> 28 );
		*p++ = encoded_hex_char( x >> 24 );
		*p++ = encoded_hex_char( x >> 20 );
		*p++ = encoded_hex_char( x >> 16 );
		*p++ = encoded_hex_char( x >> 12 );
		*p++ = encoded_hex_char( x >>  8 );
		*p++ = encoded_hex_char( x >>  4 );
		*p++ = encoded_hex_char( x >>  0 );
		
		return p;
	}
	
	char* encode_32_bit_HEX( unsigned x, char* p )
	{
		*p++ = encoded_HEX_char( x >> 28 );
		*p++ = encoded_HEX_char( x >> 24 );
		*p++ = encoded_HEX_char( x >> 20 );
		*p++ = encoded_HEX_char( x >> 16 );
		*p++ = encoded_HEX_char( x >> 12 );
		*p++ = encoded_HEX_char( x >>  8 );
		*p++ = encoded_HEX_char( x >>  4 );
		*p++ = encoded_HEX_char( x >>  0 );
		
		return p;
	}
	
	char* inscribe_n_hex_digits( char* p, unsigned long x, unsigned short n )
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
		
		return p;
	}
	
	char* inscribe_n_HEX_digits( char* p, unsigned long x, unsigned short n )
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
				*p++ = encoded_HEX_char( x >> 28 );
				// fall through
			
			case 7:
				*p++ = encoded_HEX_char( x >> 24 );
				// fall through
			
			case 6:
				*p++ = encoded_HEX_char( x >> 20 );
				// fall through
			
			case 5:
				*p++ = encoded_HEX_char( x >> 16 );
				// fall through
			
			case 4:
				*p++ = encoded_HEX_char( x >> 12 );
				// fall through
			
			case 3:
				*p++ = encoded_HEX_char( x >>  8 );
				// fall through
			
			case 2:
				*p++ = encoded_HEX_char( x >>  4 );
				// fall through
			
			case 1:
				*p   = encoded_HEX_char( x       );
				// fall through
			
			case 0:
				break;
		}
		
		return p;
	}
	
	char* hexpcpy_lower( char* out, const void* in, unsigned long n )
	{
		const char* p = (const char*) in;
		
		while ( n-- )
		{
			const unsigned char c = *p++;
			
			*out++ = encoded_hex_char( c >> 4 );
			*out++ = encoded_hex_char( c >> 0 );
		}
		
		return out;
	}
	
	char* hexpcpy_upper( char* out, const void* in, unsigned long n )
	{
		const char* p = (const char*) in;
		
		while ( n-- )
		{
			const unsigned char c = *p++;
			
			*out++ = encoded_HEX_char( c >> 4 );
			*out++ = encoded_HEX_char( c >> 0 );
		}
		
		return out;
	}
	
}
