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

}
}

#endif
