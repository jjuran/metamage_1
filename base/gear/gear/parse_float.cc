/*
	gear/parse_float.cc
	-------------------
*/

#include "gear/parse_float.hh"

// iota
#include "iota/char_types.hh"


namespace gear
{
	
	float parse_float( const char** pp )
	{
		const char*& p = *pp;
		
		float result = 0.0;
		
		while ( iota::is_space( *p ) )
		{
			++p;
		}
		
		const bool negative = *p == '-';
		
		p += negative;
		
		while ( iota::is_digit( *p ) )
		{
			result = result * 10 + (*p - '0');
			
			++p;
		}
		
		if ( *p == '.' )
		{
			float divisor = 1;
			
			while ( iota::is_digit( *++p ) )
			{
				divisor *= 10;
				
				result += (*p - '0') / divisor;
			}
		}
		
		if ( negative )
		{
			result = -result;
		}
		
		return result;
	}
	
}
