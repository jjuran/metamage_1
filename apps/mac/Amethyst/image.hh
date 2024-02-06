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


CGImageRef
create_monochrome_image( const void* base, int stride, int w, int h, int bpp );

#endif
