/*
	self_masked_drawing.cc
	----------------------
*/

#include "mac_cg/self_masked_drawing.hh"

#if TARGET_RT_MAC_MACHO

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QDOFFSCREEN_H
#include "missing/QDOffscreen.h"
#endif
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif

// mac-cg-utils
#include "mac_cg/colorspaces.hh"
#include "mac_cg/images.hh"


#pragma exceptions off


namespace mac {
namespace cg  {

#if ! __LP64__

static
CGImageRef create_self_masked_image( PixMapHandle pix )
{
	using mac::cg::create_image_from_data;
	using mac::cg::generic_or_device_RGB;
	
	Ptr   baseAddr = GetPixBaseAddr( pix );
	short rowBytes = GetPixRowBytes( pix );
	
	const Rect& bounds = pix[0]->bounds;
	
	short width  = bounds.right - bounds.left;
	short height = bounds.bottom - bounds.top;
	
	long n_pixels = rowBytes * height / sizeof (UInt32);
	
	Byte* p = (Byte*) baseAddr;
	
	for ( long i = 0;  i < n_pixels;  ++i )
	{
		if ( !~*(const UInt32*) p )
		{
			*p = 0x00;
		}
		
		p += 4;
	}
	
	CGImageRef image = NULL;
	
	if ( CGColorSpaceRef rgb = generic_or_device_RGB() )
	{
		image = create_image_from_data( width,
		                                height,
		                                8,   // bits per component
		                                32,  // bits per pixel
		                                rowBytes,
		                                rgb,
		                                kCGImageAlphaFirst,
		                                baseAddr );
	}
	
	return image;
}

self_masked_drawing::self_masked_drawing( CGContextRef   context,
                                          const CGRect&  bounds )
{
	its_context = context;
	its_CGRect  = bounds;
	
	its_QDRect.left   = (short)  bounds.origin.x;
	its_QDRect.top    = (short)  bounds.origin.y;
	its_QDRect.right  = (short) (bounds.origin.x + bounds.size.width);
	its_QDRect.bottom = (short) (bounds.origin.y + bounds.size.height);
	
	its_gworld = NULL;
	
	QDErr err = NewGWorld( &its_gworld, 32, &its_QDRect, NULL, NULL, 0 );
	
	if ( err == noErr )
	{
		GetPort( &its_saved_port );
		
		SetPort( its_gworld );
		
		EraseRect( &its_QDRect );
	}
}

self_masked_drawing::~self_masked_drawing()
{
	if ( its_gworld )
	{
		SetPort( its_saved_port );
		
		PixMapHandle pix = GetGWorldPixMap( its_gworld );
		
		if ( CGImageRef image = create_self_masked_image( pix ) )
		{
			HIViewDrawCGImage( its_context, &its_CGRect, image );
			
			CFRelease( image );
		}
		
		DisposeGWorld( its_gworld );
	}
}

#else  // #if ! __LP64__

int dummy;

#endif

}
}

#endif  // #if TARGET_RT_MAC_MACHO
