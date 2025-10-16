/*
	images.cc
	---------
*/

#include "mac_cg/images.hh"

// mac-cg-utils
#include "mac_cg/data.hh"


namespace mac {
namespace cg  {

CGImageRef create_image_from_data( size_t           width,
                                   size_t           height,
                                   size_t           degree,
                                   size_t           weight,
                                   size_t           stride,
                                   CGColorSpaceRef  colorSpace,
                                   CGBitmapInfo     bitmapInfo,
                                   void*            baseAddr )
{
	CGDataProviderRef dataProvider = make_data_provider_copy( baseAddr,
	                                                          height * stride );
	
	if ( ! dataProvider )  return NULL;
	
	CGImageRef image = CGImageCreate( width,
	                                  height,
	                                  degree,  // bits per component
	                                  weight,  // bits per pixel
	                                  stride,
	                                  colorSpace,
	                                  bitmapInfo,
	                                  dataProvider,
	                                  NULL,
	                                  false,
	                                  kCGRenderingIntentDefault );
	
	CFRelease( dataProvider );
	
	return image;
}

CGImageRef
create_simple_image( size_t           width,
                     size_t           height,
                     size_t           weight,
                     size_t           stride,
                     CGColorSpaceRef  colorSpace,
                     void*            baseAddr )
{
	return create_image_from_data( width,
	                               height,
	                               weight,  // bits per component
	                               weight,  // bits per pixel
	                               stride,
	                               colorSpace,
	                               kCGImageAlphaNone,
	                               baseAddr );
}

}
}
