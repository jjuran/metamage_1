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


typedef void (*drawer)( CGContextRef c, size_t width, size_t height );

CGContextRef create_bitmap_context( size_t length );

void write_PNG_image( CGContextRef c, const char* path );
void write_JPEG_2000( CGContextRef c, const char* path );
void write_thumbnail( CGContextRef c, const char* path );
void write_mask     ( CGContextRef c, const char* path );

void write_PDF_document( drawer draw, int width, int height, const char* path );


#endif
