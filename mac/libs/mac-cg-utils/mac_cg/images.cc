/*
	images.cc
	---------
*/

#include "mac_cg/images.hh"

// Standard C
#include <stdlib.h>
#include <string.h>


namespace mac {
namespace cg  {

static
void release_data( void* info, const void* data, size_t size )
{
	free( const_cast< void* >( data ) );
}

static
CGDataProviderRef make_data_provider( void* data, size_t size )
{
	void* buffer = malloc( size );
	
	if ( buffer == NULL )
	{
		return NULL;
	}
	
	memcpy( buffer, data, size );
	
	CGDataProviderRef result = CGDataProviderCreateWithData( NULL,
	                                                         buffer,
	                                                         size,
	                                                         &release_data );
	
	if ( result == NULL )
	{
		free( buffer );
	}
	
	return result;
}

CGImageRef create_image_from_data( size_t           width,
                                   size_t           height,
                                   size_t           degree,
                                   size_t           weight,
                                   size_t           stride,
                                   CGColorSpaceRef  colorSpace,
                                   CGBitmapInfo     bitmapInfo,
                                   void*            baseAddr )
{
	CGDataProviderRef dataProvider = make_data_provider( baseAddr,
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
