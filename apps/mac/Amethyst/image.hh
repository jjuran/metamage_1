/*
	image.hh
	--------
*/

#ifndef IMAGE_HH
#define IMAGE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


CGImageRef create_bilevel_image( const void* base, int rowBytes, int w, int h );

#endif
