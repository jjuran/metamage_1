/*
	gear/parse_float.cc
	-------------------
*/

#include "gear/parse_float.hh"

// Standard C/C++
#include <cctype>


namespace gear
{
	
	float parse_float( const char** pp )
	{
		const char*& p = *pp;
		
		float result = 0.0;
		
		while ( std::isspace( *p ) )
		{
			++p;
		}
		
		const bool negative = *p == '-';
		
		p += negative;
		
		while ( std::isdigit( *p ) )
		{
			result = result * 10 + (*p - '0');
			
			++p;
		}
		
		if ( *p == '.' )
		{
			float divisor = 1;
			
			while ( std::isdigit( *++p ) )
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

