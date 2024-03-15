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
CGColorSpaceRef RGBColorSpace()
{
	static CGColorSpaceRef colorSpace = 
	
#ifdef MAC_OS_X_VERSION_10_4
	
	true ? CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB ) :
	
#endif
	
	CGColorSpaceCreateDeviceRGB();
	
	return colorSpace;
}

static
CGColorSpaceRef IndexedColorSpace( const ColorSpec* colors )
{
	CGColorSpaceRef colorSpace = RGBColorSpace();
	
	CGColorSpaceRetain( colorSpace );
	
	short clut_max = 255;
	
	uint8_t* table = (uint8_t*) alloca( 3 * (clut_max + 1) );
	
	uint8_t* q = table;
	
	const uint16_t* p = (const uint16_t*) colors;
	
	for ( int i = 0;  i <= clut_max;  ++i )
	{
		++p;  // skip value
		
		*q++ = *p++ >> 8;
		*q++ = *p++ >> 8;
		*q++ = *p++ >> 8;
	}
	
	CGColorSpaceRef index = CGColorSpaceCreateIndexed( colorSpace,
	                                                   clut_max,
	                                                   table );
	
	CGColorSpaceRelease( colorSpace );
	
	return index;
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
CGDataProviderRef make_data_provider( char* data, size_t size, bool invert )
{
	void* buffer = malloc( size );
	
	if ( buffer == NULL )
	{
		return NULL;
	}
	
	if ( invert )
	{
		inverted_copy( buffer, data, size );
	}
	else
	{
		memcpy( buffer, data, size );
	}
	
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
                                   char*            baseAddr,
                                   bool             invert )
{
	CGDataProviderRef dataProvider = make_data_provider( baseAddr,
	                                                     height * stride,
	                                                     invert );
	
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
	                               (char*) base,
	                               true );
}

CGImageRef image_from_indexed_data( const void*       baseAddr,
                                    size_t            stride,
                                    size_t            width,
                                    size_t            height,
                                    size_t            weight,
                                    const ColorSpec*  colors )
{
	CGColorSpaceRef colorSpace = IndexedColorSpace( colors );
	
	CGImageRef image = create_image_from_data( width,
	                                           height,
	                                           weight,  // bits per component
	                                           weight,  // bits per pixel
	                                           stride,
	                                           colorSpace,
	                                           kCGImageAlphaNone,
	                                           (char*) baseAddr,
	                                           false );
	
	CGColorSpaceRelease( colorSpace );
	
	return image;
}
