/*
	image.cc
	--------
*/

#include "image.hh"

// Standard C
#include <stdlib.h>
#include <string.h>


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif

#ifndef MAC_OS_X_VERSION_10_5
typedef float CGFloat;
#endif


static
CGColorSpaceRef GrayColorSpace()
{
	static CGColorSpaceRef colorSpace = 
	
#ifdef MAC_OS_X_VERSION_10_4
	
	true ? CGColorSpaceCreateWithName( kCGColorSpaceGenericGray ) :
	
#endif
	
	CGColorSpaceCreateDeviceGray();
	
	return colorSpace;
}

static
void release_data( void* info, const void* data, size_t size )
{
	free( const_cast< void* >( data ) );
}

static
CGDataProviderRef make_data_provider( char* data, size_t size )
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
	
	if ( result )
	{
		return result;
	}
	
	free( buffer );
	
	return NULL;
}

static
CGImageRef create_image_from_data( size_t           width,
                                   size_t           height,
                                   size_t           degree,
                                   size_t           weight,
                                   size_t           stride,
                                   CGColorSpaceRef  colorSpace,
                                   CGBitmapInfo     bitmapInfo,
                                   char*            baseAddr )
{
	CGDataProviderRef dataProvider = make_data_provider( baseAddr,
	                                                     height * stride );
	
	if ( dataProvider )
	{
		const CGFloat decode[] = { 1.0, 0.0 };
		
		CGImageRef image = CGImageCreate( width,
		                                  height,
		                                  degree,  // bits per component
		                                  weight,  // bits per pixel
		                                  stride,
		                                  colorSpace,
		                                  bitmapInfo,
		                                  dataProvider,
		                                  decode,
		                                  false,
		                                  kCGRenderingIntentDefault );
		
		CFRelease( dataProvider );
		
		return image;
	}
	
	return NULL;
}

CGImageRef
create_monochrome_image( const void* base, int stride, int w, int h, int bpp )
{
	return create_image_from_data( w,
	                               h,
	                               bpp,  // bits per component
	                               bpp,  // bits per pixel
	                               stride,
	                               GrayColorSpace(),
	                               kCGImageAlphaNone,
	                               (char*) base );
}
