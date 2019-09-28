/*
	image.cc
	--------
*/

#include "image.hh"

// Standard C
#include <stdlib.h>


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
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
void inverted_copy( void* dst, const void* src, size_t n )
{
	uint32_t const* p   = (const uint32_t*) src;
	uint32_t const* end = (const uint32_t*) src + n / 4;
	
	uint32_t* q = (uint32_t*) dst;
	
	while ( p < end )
	{
		*q++ = ~*p++;
	}
}

static
CGDataProviderRef make_data_provider( char* data, size_t size )
{
	void* buffer = malloc( size );
	
	if ( buffer == NULL )
	{
		return NULL;
	}
	
	inverted_copy( buffer, data, size );
	
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
	
	return NULL;
}

CGImageRef create_bilevel_image( const void* base, int rowBytes, int w, int h )
{
	return create_image_from_data( w,
	                               h,
	                               1,  // bits per component
	                               1,  // bits per pixel
	                               rowBytes,
	                               GrayColorSpace(),
	                               kCGImageAlphaNone,
	                               (char*) base );
}
