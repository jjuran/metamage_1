/*
	CGQuickDraw.cc
	--------------
*/

#include "CGQuickDraw.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>
#include <string.h>

// mac-qd-utils
#include "mac_qd/is_monochrome.hh"


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif


using mac::qd::is_monochrome;


static
CGColorSpaceRef GrayColorSpace()
{
	static CGColorSpaceRef colorSpace =
	
#ifdef MAC_OS_X_VERSION_10_4
	
	CGColorSpaceCreateWithName( kCGColorSpaceGenericGray )
	
#else
	
	CGColorSpaceCreateDeviceGray()
	
#endif
	;
	
	return colorSpace;
}

static
CGColorSpaceRef RGBColorSpace()
{
	static CGColorSpaceRef colorSpace =
	
#ifdef MAC_OS_X_VERSION_10_4
	
	CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB )
	
#else
	
	CGColorSpaceCreateDeviceRGB()
	
#endif
	;
	
	return colorSpace;
}

static
void release_data( void* info, const void* data, size_t size )
{
	free( const_cast< void* >( data ) );
}

typedef void (*copier)( void* dst, const void* src, size_t n );

static
void straight_copy( void* dst, const void* src, size_t n )
{
	memcpy( dst, src, n );
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
CGDataProviderRef make_data_provider( char* data, size_t size, copier cpy )
{
	void* buffer = data;
	
	if ( cpy )
	{
		buffer = malloc( size );
		
		if ( buffer == NULL )
		{
			return NULL;
		}
		
		cpy( buffer, data, size );
	}
	
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

static
CGBitmapInfo BitmapInfo_from_PixMap( const PixMap& pixmap )
{
	/*
		This function is only called for direct color pixels.
		Weight is either 16 or 32.  It returns kCGImageAlphaNoneSkipFirst
		unless pixmap.pixelFormat has been set to a recognized value which
		requires a different configuration.
	*/
	
#ifdef MAC_OS_X_VERSION_10_4
	
	switch ( pixmap.pixelFormat )
	{
		case k16LE555PixelFormat:  // Mac OS X
			return kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder16Little;
		
		case k32BGRAPixelFormat:  // Mac OS X, Linux, Android (e.g Nexus S)
			return kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little;
		
		case k32RGBAPixelFormat:  // Android (e.g. Nexus 4)
			return kCGImageAlphaNoneSkipLast;
		
		default:
			break;
	}
	
#endif
	
	return kCGImageAlphaNoneSkipFirst;
}

static
CGImageRef image_from_data( size_t           width,
                            size_t           height,
                            size_t           degree,
                            size_t           weight,
                            size_t           stride,
                            CGColorSpaceRef  colorSpace,
                            CGBitmapInfo     bitmapInfo,
                            char*            baseAddr,
                            copier           cpy )
{
	CGDataProviderRef dataProvider = make_data_provider( baseAddr,
	                                                     height * stride,
	                                                     cpy );
	
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

static
CGImageRef image_from_monochrome_data( size_t  width,
                                       size_t  height,
                                       size_t  weight,
                                       size_t  stride,
                                       char*   baseAddr )
{
	return image_from_data( width,
	                        height,
	                        weight,  // bits per component
	                        weight,  // bits per pixel
	                        stride,
	                        GrayColorSpace(),
	                        kCGImageAlphaNone,
	                        baseAddr,
	                        &inverted_copy );
}

static
CGImageRef image_from_indexed_data( size_t            width,
                                    size_t            height,
                                    size_t            weight,
                                    size_t            stride,
                                    const ColorSpec*  colors,
                                    char*             baseAddr )
{
	CGImageRef result = NULL;
	
	const size_t new_stride = width * 4;
	
	if ( uint32_t* pixel_data = (uint32_t*) malloc( height * new_stride ) )
	{
		uint32_t* p = pixel_data;
		
		Ptr row = baseAddr;
		
		for ( int i = 0;  i < height;  ++i )
		{
			Ptr q = row;
			
			for ( int j = 0;  j < width;  j += 8u / weight )
			{
				uint8_t pixel = *q++;
				
				if ( weight == 2 )
				{
					const ColorSpec& color = colors[ pixel >> 6 ];
					
					*p++ = color.rgb.red   >> 8 << 16
					     | color.rgb.green >> 8 <<  8
					     | color.rgb.blue  >> 8;
					
					pixel &= 0x3F;
				}
				
				if ( weight <= 4 )
				{
					const ColorSpec& color = colors[ pixel >> 4 ];
					
					*p++ = color.rgb.red   >> 8 << 16
					     | color.rgb.green >> 8 <<  8
					     | color.rgb.blue  >> 8;
					
					pixel &= 0xF;
				}
				
				if ( weight == 2 )
				{
					const ColorSpec& color = colors[ pixel >> 2 ];
					
					*p++ = color.rgb.red   >> 8 << 16
					     | color.rgb.green >> 8 <<  8
					     | color.rgb.blue  >> 8;
					
					pixel &= 0x03;
				}
				
				const ColorSpec& color = colors[ pixel ];
				
				*p++ = color.rgb.red   >> 8 << 16
				     | color.rgb.green >> 8 <<  8
				     | color.rgb.blue  >> 8;
			}
			
			row += stride;
		}
		
		CGBitmapInfo bitmapInfo = kCGImageAlphaNoneSkipFirst;
		
	#if TARGET_RT_LITTLE_ENDIAN
		
		bitmapInfo |= kCGBitmapByteOrder32Little;
		
	#endif
		
		result = image_from_data( width,
		                          height,
		                          8,   // bits per component
		                          32,  // bits per pixel
		                          new_stride,
		                          RGBColorSpace(),
		                          bitmapInfo,
		                          (Ptr) pixel_data,
		                          NULL );
	}
	
	return result;
}


static inline
CGImageRef image_from_RGB_data( size_t        width,
                                size_t        height,
                                size_t        degree,
                                size_t        weight,
                                size_t        stride,
                                CGBitmapInfo  bitmapInfo,
                                char*         baseAddr )
{
	return image_from_data( width,
	                        height,
	                        degree,  // bits per component
	                        weight,  // bits per pixel
	                        stride,
	                        RGBColorSpace(),
	                        bitmapInfo,
	                        baseAddr,
	                        &straight_copy );
}

CGImageRef CreateCGImageFromPixMap( const PixMap& pixmap )
{
	long rowBytes = pixmap.rowBytes & 0x3FFF;
	
	const short width  = pixmap.bounds.right - pixmap.bounds.left;
	const short height = pixmap.bounds.bottom - pixmap.bounds.top;
	
	if ( const bool direct = pixmap.pixelType != 0 )
	{
		return image_from_RGB_data( width,
		                            height,
		                            pixmap.cmpSize,
		                            pixmap.pixelSize,
		                            rowBytes,
		                            BitmapInfo_from_PixMap( pixmap ),
		                            pixmap.baseAddr );
	}
	else if ( const bool monochrome = is_monochrome( pixmap ) )
	{
		return image_from_monochrome_data( width,
		                                   height,
		                                   pixmap.pixelSize,
		                                   rowBytes,
		                                   pixmap.baseAddr );
	}
	else if ( CTabHandle ctab = pixmap.pmTable )
	{
		return image_from_indexed_data( width,
		                                height,
		                                pixmap.pixelSize,
		                                rowBytes,
		                                ctab[0]->ctTable,
		                                pixmap.baseAddr );
	}
	
	return NULL;
}
