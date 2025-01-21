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
#include "mac_qd/is_monochrome.hh"

// CGQuickDraw
#include "CGQuickDraw.hh"

// Nitrogen
#include "Nitrogen/CGColorSpace.hh"
#include "Nitrogen/CGDataProvider.hh"
#include "Nitrogen/CGImage.hh"
#include "Nitrogen/QDOffscreen.hh"


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	using mac::qd::is_monochrome;
	
	
	static
	n::owned< CGColorSpaceRef > GrayColorSpace()
	{
	#ifdef MAC_OS_X_VERSION_10_4
		
		return N::CGColorSpaceCreateWithName( kCGColorSpaceGenericGray );
		
	#endif
		
		return N::CGColorSpaceCreateDeviceGray();
	}
	
	static
	void release_data( void* info, const void* data, size_t size )
	{
		::operator delete( const_cast< void* >( data ) );
	}
	
	typedef void (*copier)( void* dst, const void* src, size_t n );
	
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
	n::owned< CGDataProviderRef >
	//
	make_data_provider( char* data, size_t size, copier cpy )
	{
		void* buffer = ::operator new( size );
		
		cpy( buffer, data, size );
		
		return N::CGDataProviderCreateWithData( buffer,
		                                        size,
		                                        &release_data,
		                                        &release_data );
	}
	
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
	n::owned< CGImageRef > image_from_data( size_t           width,
	                                        size_t           height,
	                                        size_t           degree,
	                                        size_t           weight,
	                                        size_t           stride,
	                                        CGColorSpaceRef  colorSpace,
	                                        CGBitmapInfo     bitmapInfo,
	                                        char*            baseAddr,
	                                        copier           cpy )
	{
		return N::CGImageCreate( width,
		                         height,
		                         degree,  // bits per component
		                         weight,  // bits per pixel
		                         stride,
		                         colorSpace,
		                         bitmapInfo,
		                         make_data_provider( baseAddr,
		                                             height * stride,
		                                             cpy ) );
	}
	
	static
	n::owned< CGImageRef > image_from_monochrome_data( size_t  width,
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
	n::owned< CGImageRef > image_from_bitmap( const BitMap& bitmap )
	{
		const short width  = bitmap.bounds.right - bitmap.bounds.left;
		const short height = bitmap.bounds.bottom - bitmap.bounds.top;
		const short stride = bitmap.rowBytes;
		
		return image_from_monochrome_data( width,
		                                   height,
		                                   1,  // weight
		                                   stride,
		                                   bitmap.baseAddr );
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
		
		short converting = ! is_monochrome( **pix )  &&  pix[0]->pixelSize <= 8;
		
	#ifdef MAC_OS_X_VERSION_10_4
		
		const uint32_t pixelFormat = pix[0]->pixelFormat;
		
		if ( TARGET_RT_LITTLE_ENDIAN  &&  pixelFormat == k16LE565PixelFormat )
		{
			converting = true;
		}
		
	#endif
		
		if ( converting )
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
