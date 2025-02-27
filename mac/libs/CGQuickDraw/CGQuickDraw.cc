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
#ifndef MAC_OS_X_VERSION_10_7
#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>
#include <string.h>

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QDOFFSCREEN_H
#include "missing/QDOffscreen.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/is_monochrome.hh"


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif

#ifndef MAC_OS_X_VERSION_10_5
typedef float CGFloat;
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
                            const CGFloat    decode[] = NULL )
{
	CGDataProviderRef dataProvider = make_data_provider( baseAddr,
	                                                     height * stride );
	
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

static
CGImageRef image_from_monochrome_data( size_t  width,
                                       size_t  height,
                                       size_t  weight,
                                       size_t  stride,
                                       char*   baseAddr )
{
	const CGFloat decode[] = { 1.0, 0.0 };
	
	return image_from_data( width,
	                        height,
	                        weight,  // bits per component
	                        weight,  // bits per pixel
	                        stride,
	                        GrayColorSpace(),
	                        kCGImageAlphaNone,
	                        baseAddr,
	                        decode );
}

static
CGImageRef image_from_indexed_data( size_t            width,
                                    size_t            height,
                                    size_t            weight,
                                    size_t            stride,
                                    const ColorSpec*  colors,
                                    size_t            count,
                                    char*             baseAddr )
{
	Byte* table = (Byte*) alloca( 3 * count );
	
	Byte* p = table;
	
	for ( int i = 0;  i < count;  ++i )
	{
		const RGBColor& c = colors[ i ].rgb;
		
		*p++ = c.red   >> 8;
		*p++ = c.green >> 8;
		*p++ = c.blue  >> 8;
	}
	
	CGColorSpaceRef rgb = RGBColorSpace();
	
	CGColorSpaceRef index = CGColorSpaceCreateIndexed( rgb, count - 1, table );
	
	CGImageRef result = image_from_data( width,
	                                     height,
	                                     weight,  // bits per component
	                                     weight,  // bits per pixel
	                                     stride,
	                                     index,
	                                     kCGImageAlphaNone,
	                                     baseAddr );
	
	CGColorSpaceRelease( index );
	
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
	                        baseAddr );
}

CGImageRef CreateCGImageFromBitMap( const BitMap& bitmap )
{
	const short width  = bitmap.bounds.right - bitmap.bounds.left;
	const short height = bitmap.bounds.bottom - bitmap.bounds.top;
	
	return image_from_monochrome_data( width,
	                                   height,
	                                   1,  // weight / depth
	                                   bitmap.rowBytes,
	                                   bitmap.baseAddr );
}

#if ! __LP64__

CGImageRef CreateCGImageFromPixMap( PixMapHandle pix )
{
	const PixMap& pixmap = **pix;
	
	Ptr baseAddr  = GetPixBaseAddr( pix );
	long rowBytes = GetPixRowBytes( pix );
	
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
		                            baseAddr );
	}
	else if ( const bool monochrome = is_monochrome( pixmap ) )
	{
		return image_from_monochrome_data( width,
		                                   height,
		                                   pixmap.pixelSize,
		                                   rowBytes,
		                                   baseAddr );
	}
	else if ( CTabHandle ctab = pixmap.pmTable )
	{
		return image_from_indexed_data( width,
		                                height,
		                                pixmap.pixelSize,
		                                rowBytes,
		                                ctab[0]->ctTable,
		                                ctab[0]->ctSize,
		                                baseAddr );
	}
	
	return NULL;
}

#else

int dummy;

#endif
