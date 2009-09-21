/*	===================
 *	iota/hexidecimal.cc
 *	===================
 */

#include "iota/hexidecimal.hh"


namespace iota
{
	
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

