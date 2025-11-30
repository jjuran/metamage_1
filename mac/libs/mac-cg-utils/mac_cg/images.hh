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

// Mac OS
#ifndef CGIMAGE_H_
#ifndef __CGIMAGE__
#include <CGImage.h>
#endif
#endif

// Standard C
#include <stddef.h>

// mac-cg-utils
#include "mac_cg/colorspaces.hh"


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif


namespace mac {
namespace cg  {

enum
{
#ifdef __LITTLE_ENDIAN__
	
	kBitmapInfo_32bit = kCGBitmapByteOrder32Little,
	
#else
	
	kBitmapInfo_32bit = 0,
	
#endif
	
	kBitmapInfo_ARGB_32 = kBitmapInfo_32bit | kCGImageAlphaFirst,
	kBitmapInfo_xRGB_32 = kBitmapInfo_32bit | kCGImageAlphaNoneSkipFirst,
};

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
CGImageRef create_gray_light_image( size_t  width,
                                    size_t  height,
                                    size_t  weight,
                                    size_t  stride,
                                    void*   baseAddr )
{
	return create_simple_image( width,
	                            height,
	                            weight,
	                            stride,
	                            generic_or_device_gray(),
	                            baseAddr );
}

inline
CGImageRef create_gray_paint_image( size_t  width,
                                    size_t  height,
                                    size_t  weight,
                                    size_t  stride,
                                    void*   baseAddr )
{
	CGColorSpaceRef black_on_white = create_inverted_grayscale( 1 << weight );
	
	CGImageRef image = create_simple_image( width,
	                                        height,
	                                        weight,
	                                        stride,
	                                        black_on_white,
	                                        baseAddr );
	
	CGColorSpaceRelease( black_on_white );
	
	return image;
}

inline
CGImageRef create_RGB_image( size_t        width,
                             size_t        height,
                             size_t        degree,
                             size_t        weight,
                             size_t        stride,
                             CGBitmapInfo  bitmapInfo,
                             void*         baseAddr )
{
	return create_image_from_data( width,
	                               height,
	                               degree,  // bits per component
	                               weight,  // bits per pixel
	                               stride,
	                               generic_or_device_RGB(),
	                               bitmapInfo,
	                               baseAddr );
}

inline
CGImageRef create_1555_image( size_t        width,
                              size_t        height,
                              size_t        stride,
                              CGBitmapInfo  bitmapInfo,
                              void*         baseAddr )
{
	return create_RGB_image( width,
	                         height,
	                         5,   // bits per component
	                         16,  // bits per pixel
	                         stride,
	                         bitmapInfo,
	                         baseAddr );
}

inline
CGImageRef create_8888_image( size_t        width,
                              size_t        height,
                              size_t        stride,
                              CGBitmapInfo  bitmapInfo,
                              void*         baseAddr )
{
	return create_RGB_image( width,
	                         height,
	                         8,   // bits per component
	                         32,  // bits per pixel
	                         stride,
	                         bitmapInfo,
	                         baseAddr );
}

inline
CGImageRef
create_xRGB_1555_BE_image( size_t  width,
                           size_t  height,
                           size_t  stride,
                           void*   baseAddr )
{
	return create_1555_image( width,
	                          height,
	                          stride,
	                          kCGImageAlphaNoneSkipFirst,
	                          baseAddr );
}

inline
CGImageRef
create_xRGB_8888_BE_image( size_t  width,
                           size_t  height,
                           size_t  stride,
                           void*   baseAddr )
{
	return create_8888_image( width,
	                          height,
	                          stride,
	                          kCGImageAlphaNoneSkipFirst,
	                          baseAddr );
}

inline
CGImageRef
create_xRGB_8888_image( size_t  width,
                        size_t  height,
                        size_t  stride,
                        void*   baseAddr )
{
	return create_8888_image( width,
	                          height,
	                          stride,
	                          (CGBitmapInfo) kBitmapInfo_xRGB_32,
	                          baseAddr );
}

}
}

#endif
