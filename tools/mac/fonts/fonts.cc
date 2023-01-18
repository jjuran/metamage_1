/*
	fonts.cc
	--------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif

// Standard C
#include <stdio.h>

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_FONTS_H
#include "missing/Fonts.h"
#endif
#endif

// chars
#include "conv/mac_utf8.hh"


#pragma exceptions off


int main( int argc, char** argv )
{
	Str255 mac_name;
	
	UInt16 i = 0;
	
	do
	{
		GetFontName( i, mac_name );
		
		unsigned char* p = mac_name;
		
		if ( unsigned len = *p++ )
		{
			char utf[ 255 * 3 + 1 ];  // worst case
			
			size_t n = conv::utf8_from_mac( utf, sizeof utf, (char*) p, len );
			
			utf[ n ] = '\0';
			
			printf( "%d: %s\n", (SInt16) i, utf );
		}
	}
	while ( ++i );
	
	return 0;
}
