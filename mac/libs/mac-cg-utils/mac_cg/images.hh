/*
	images.hh
	---------
*/

#ifndef MACCG_IMAGES_HH
#define MACCG_IMAGES_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Standard C
#include <stddef.h>


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif


namespace mac {
namespace cg  {

CGImageRef create_image_from_data( size_t           width,
                                   size_t           height,
                                   size_t           degree,
                                   size_t           weight,
                                   size_t           stride,
                                   CGColorSpaceRef  colorSpace,
                                   CGBitmapInfo     bitmapInfo,
                                   void*            baseAddr );

CGImageRef
create_simple_image( size_t           width,
                     size_t           height,
                     size_t           weight,
                     size_t           stride,
                     CGColorSpaceRef  colorSpace,
                     void*            baseAddr );

inline
CGImageRef
create_xRGB_1555_BE_image( size_t           width,
                           size_t           height,
                           size_t           stride,
                           CGColorSpaceRef  colorSpace,
                           void*            baseAddr )
{
	return create_image_from_data( width,
	                               height,
	                               5,   // bits per component
	                               16,  // bits per pixel
	                               stride,
	                               colorSpace,
	                               kCGImageAlphaNoneSkipFirst,
	                               baseAddr );
}

inline
CGImageRef
create_xRGB_8888_BE_image( size_t           width,
                           size_t           height,
                           size_t           stride,
                           CGColorSpaceRef  colorSpace,
                           void*            baseAddr )
{
	return create_image_from_data( width,
	                               height,
	                               8,   // bits per component
	                               32,  // bits per pixel
	                               stride,
	                               colorSpace,
	                               kCGImageAlphaNoneSkipFirst,
	                               baseAddr );
}

}
}

#endif
