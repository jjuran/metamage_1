/*
	utils.hh
	--------
*/

#ifndef ICONGEN_UTILS_HH
#define ICONGEN_UTILS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


CGContextRef create_bitmap_context( size_t length );

void write_PNG_image( CGContextRef c, const char* path );
void write_thumbnail( CGContextRef c, const char* path );
void write_mask     ( CGContextRef c, const char* path );


#endif
