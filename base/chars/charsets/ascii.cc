/*
	ascii.cc
	--------
*/

#include "charsets/ascii.hh"


namespace chars
{
	
	const char* find_non_ascii( const char* begin,
	                            const char* end,
	                            const char* fallback )
	{
		while ( begin < end )
		{
			signed char c = *begin++;
			
			if ( c < 0 )
			{
				return --begin;
			}
		}
		
		return fallback;
	}
	
}
