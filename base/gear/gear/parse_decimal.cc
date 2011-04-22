/*
	gear/parse_decimal.cc
	---------------------
*/

#include "gear/parse_decimal.hh"

// Standard C/C++
#include <cctype>


namespace gear
{
	
	unsigned parse_unsigned_decimal( const char **pp )
	{
		const char*& p = *pp;
		
		unsigned result = 0;
		
		while ( std::isdigit( *p ) )
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
	
}

