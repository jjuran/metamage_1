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

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QDOFFSCREEN_H
#include "missing/QDOffscreen.h"
#endif
#endif

// mac-cg-utils
#include "mac_cg/colorspaces.hh"
#include "mac_cg/images.hh"

// mac-qd-utils
#include "mac_qd/is_monochrome.hh"


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif


using mac::qd::is_monochrome;


static
CGImageRef image_from_monochrome_data( size_t  width,
                                       size_t  height,
                                       size_t  weight,
                                       size_t  stride,
                                       char*   baseAddr )
{
	using mac::cg::create_inverted_grayscale;
	using mac::cg::create_simple_image;
	
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

static
CGImageRef image_from_indexed_data( size_t            width,
                                    size_t            height,
                                    size_t            weight,
                                    size_t            stride,
                                    const ColorSpec*  colors,
                                    size_t            count,
                                    char*             baseAddr )
{
	using mac::cg::create_RGB_palette;
	using mac::cg::create_simple_image;
	
	CGColorSpaceRef index = create_RGB_palette( (UInt16*) colors, count );
	
	CGImageRef result = create_simple_image( width,
	                                         height,
	                                         weight,
	                                         stride,
	                                         index,
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
	using mac::cg::create_image_from_data;
	using mac::cg::generic_or_device_RGB;
	
	CGColorSpaceRef rgb = generic_or_device_RGB();
	
	CGImageRef image = create_image_from_data( width,
	                                           height,
	                                           degree,  // bits per component
	                                           weight,  // bits per pixel
	                                           stride,
	                                           rgb,
	                                           bitmapInfo,
	                                           baseAddr );
	
	return image;
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
