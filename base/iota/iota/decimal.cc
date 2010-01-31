/*	===============
 *	iota/decimal.cc
 *	===============
 */

#include "iota/decimal.hh"

// Standard C
#include <ctype.h>


namespace iota
{
	
	unsigned parse_unsigned_decimal( const char **pp )
	{
		const char*& p = *pp;
		
		unsigned result = 0;
		
		while ( isdigit( *p ) )
		{
			result = result * 10 + *p++ - '0';
		}
		
		return result;
	}
	
	int parse_decimal( const char **pp )
	{
		const char*& p = *pp;
		
		const bool negative = *p == '-';
		
		if ( negative )
		{
			++p;
		}
		
		int result = parse_unsigned_decimal( &p );
		
		if ( negative )
		{
			result = -result;
		}
		
		return result;
	}
	
	
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

