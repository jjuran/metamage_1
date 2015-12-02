/*
	gear/mince.cc
	-------------
*/

#include "gear/mince.hh"


namespace gear
{
	
	char* mince( char* dst, const char* src, const char* end, unsigned stride )
	{
		while ( src < end )
		{
			*dst++ = *src;
			
			src += stride;
		}
		
		return dst;
	}
	
}
