// Carbonate/TextUtils.cc


#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#endif

#include "Carbonate/TextUtils.hh"

// These functions are always declared in the headers and are always extern.

extern "C"
{
	
#if TARGET_CPU_68K
	
	void CopyCStringToPascal( const char* src, Str255 dest )
	{
		Byte *const begin = dest + 1;
		Byte *const end = begin + 255;
		
		Byte* p = begin;
		
		while ( *src != '\0'  &&  p < end )
		{
			*p++ = *src++;
		}
		
		dest[ 0 ] = p - begin;
	}
	
#endif
	
}

