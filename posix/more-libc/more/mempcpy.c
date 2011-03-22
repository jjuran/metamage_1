/*
	mempcpy.c
	---------
*/

#ifndef __MC68K__

// more-libc
#include "more/string.h"


void* mempcpy( void* dest, const void* src, size_t n )
{
	char const* p = (char const*) src;
	char      * q = (char      *) dest;
	
	const char* const end = q + n;
	
	while ( q != end )
	{
		*q++ = *p++;
	}
	
	return q;
}

#endif

