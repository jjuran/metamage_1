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

CGImageRef image_from_indexed_data( const void*       baseAddr,
                                    size_t            stride,
                                    size_t            width,
                                    size_t            height,
                                    size_t            weight,
                                    const ColorSpec*  colors );

#endif
