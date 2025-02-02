/*
	HIQuickDraw.cc
	--------------
	
	Copyright 2017 Josh Juran
*/

#include "Genie/Utilities/HIQuickDraw.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// mac-qd-utils
#include "mac_qd/copy_bits.hh"

// CGQuickDraw
#include "CGQuickDraw.hh"

// Nitrogen
#include "Nitrogen/CGImage.hh"
#include "Nitrogen/QDOffscreen.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static inline
	bool is_multiple( float dst, size_t src )
	{
		if ( dst <= src )
		{
			/*
				For dst == src, return false, because the interpolation
				setting will have no effect, so we don't need to change it.
			*/
			
			return false;
		}
		
		float factor = dst / src;
		
		return factor == size_t( factor );
	}
	
	static
	OSStatus HIViewDrawCGImageUninterpolated( CGContextRef  context,
	                                          CGRect        bounds,
	                                          CGImageRef    image )
	{
		OSStatus err = 0;
		
		CGSize size = bounds.size;
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		size = CGContextConvertSizeToDeviceSpace( context, size );
		
	#endif
		
		size_t width  = CGImageGetWidth ( image );
		size_t height = CGImageGetHeight( image );
		
		const bool disable_interpolation =  is_multiple( size.width,  width  )
		                                 && is_multiple( size.height, height );
		
		if ( disable_interpolation )
		{
			CGContextSaveGState( context );
			
			CGContextSetInterpolationQuality( context, kCGInterpolationNone );
		}
		
	#ifdef MAC_OS_X_VERSION_10_2
		
		err = HIViewDrawCGImage( context, &bounds, image );
		
	#endif
		
		if ( disable_interpolation )
		{
			CGContextRestoreGState( context );
		}
		
		return err;
	}
	
	static
	n::owned< CGImageRef > image_from_bitmap( const BitMap& bitmap )
	{
		if ( CGImageRef image = CreateCGImageFromBitMap( bitmap ) )
		{
			return n::owned< CGImageRef >::seize( image );
		}
		
		return n::owned< CGImageRef >();
	}
	
	static
	n::owned< CGImageRef > image_from_pixmap( PixMapHandle pix )
	{
		if ( CGImageRef image = CreateCGImageFromPixMap( pix ) )
		{
			return n::owned< CGImageRef >::seize( image );
		}
		
		return n::owned< CGImageRef >();
	}
	
	static
	n::owned< CGImageRef > image_from_gworld( GWorldPtr gworld )
	{
		PixMapHandle pix = GetGWorldPixMap( gworld );
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		const uint32_t pixelFormat = pix[0]->pixelFormat;
		
		if ( TARGET_RT_LITTLE_ENDIAN  &&  pixelFormat == k16LE565PixelFormat )
		{
			/*
				This format is not supported by Core Graphics, but it is by
				QuickDraw.  Create a new GWorld with the same bounds and new
				depth, and call CopyBits() to transcode the pixels for us.
			*/
			
			n::owned< GWorldPtr > native = N::NewGWorld( 32, pix[0]->bounds );
			
			mac::qd::copy_bits( gworld, native );
			
			return image_from_pixmap( GetGWorldPixMap( native ) );
		}
		
	#endif
		
		return image_from_pixmap( pix );
	}
	
	void HIViewDrawBitMap( CGContextRef   context,
	                       CGRect         bounds,
	                       const BitMap&  bitmap )
	{
		OSStatus err;
		
		err = HIViewDrawCGImageUninterpolated( context,
		                                       bounds,
		                                       image_from_bitmap( bitmap ) );
	}
	
	void HIViewDrawPixMap( CGContextRef  context,
	                       CGRect        bounds,
	                       PixMapHandle  pix )
	{
		OSStatus err;
		
		err = HIViewDrawCGImageUninterpolated( context,
		                                       bounds,
		                                       image_from_pixmap( pix ) );
	}
	
	void HIViewDrawGWorld( CGContextRef  context,
	                       CGRect        bounds,
	                       GWorldPtr     gworld )
	{
		OSStatus err;
		
		err = HIViewDrawCGImageUninterpolated( context,
		                                       bounds,
		                                       image_from_gworld( gworld ) );
	}
	
}
