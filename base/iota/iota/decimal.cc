/*	===============
 *	iota/decimal.cc
 *	===============
 */

#include "iota/decimal.hh"


namespace iota
{
	
	char* inscribe_unsigned_decimal( unsigned x )
	{
		static char buffer[ sizeof "1234567890" ];
		
		char* end = inscribe_unsigned_decimal_r( x, buffer );
		
		*end = '\0';
		
		return buffer;
	}
	
	char* inscribe_decimal( int x )
	{
		static char buffer[ sizeof "-1234567890" ];
		
		char* end = inscribe_decimal_r( x, buffer );
		
		*end = '\0';
		
		return buffer;
	}
	
}

