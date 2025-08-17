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
			
			short first_real_size = 0;
			short last_real_size  = 0;
			
			bool printed = false;
			
			for ( short font_size = 1;  font_size < 128;  ++font_size )
			{
				bool real = RealFont( i, font_size );
				
				if ( real )
				{
					last_real_size = font_size;
					
					if ( ! first_real_size )
					{
						first_real_size = font_size;
					}
				}
				else
				{
					if ( first_real_size )
					{
						const char* separator = ! printed ? "    " : ", ";
						
						printf( "%s", separator );
						
						printf( "%d", first_real_size );
						
						if ( last_real_size > first_real_size )
						{
							printf( "-%d", last_real_size );
						}
						
						first_real_size = 0;
						
						printed = true;
					}
				}
			}
			
			if ( printed )
			{
				printf( "\n" );
			}
		}
	}
	while ( ++i );
	
	return 0;
}
