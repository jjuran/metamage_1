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
				*p++ = printable_hex_digit( x >> 28 );
				// fall through
			
			case 7:
				*p++ = printable_hex_digit( x >> 24 );
				// fall through
			
			case 6:
				*p++ = printable_hex_digit( x >> 20 );
				// fall through
			
			case 5:
				*p++ = printable_hex_digit( x >> 16 );
				// fall through
			
			case 4:
				*p++ = printable_hex_digit( x >> 12 );
				// fall through
			
			case 3:
				*p++ = printable_hex_digit( x >>  8 );
				// fall through
			
			case 2:
				*p++ = printable_hex_digit( x >>  4 );
				// fall through
			
			case 1:
				*p   = printable_hex_digit( x       );
				// fall through
			
			case 0:
				break;
		}
	}
	
}

