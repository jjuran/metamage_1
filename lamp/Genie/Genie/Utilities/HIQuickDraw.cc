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

// Standard C
#include <string.h>

// mac-qd-utils
#include "mac_qd/get_pix_rowBytes.hh"
#include "mac_qd/is_monochrome.hh"

// nucleus
#include "nucleus/saved.hh"

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
	
	using mac::qd::get_pix_rowBytes;
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
	n::owned< CGColorSpaceRef > RGBColorSpace()
	{
	#ifdef MAC_OS_X_VERSION_10_4
		
		return N::CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB );
		
	#endif
		
		return N::CGColorSpaceCreateDeviceRGB();
	}
	
	static
	void release_data( void* info, const void* data, size_t size )
	{
		::operator delete( const_cast< void* >( data ) );
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
	n::owned< CGImageRef > image_from_grayscale_data( size_t  width,
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
	
	static inline
	n::owned< CGImageRef > image_from_RGB_data( size_t        width,
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
	
	static
	n::owned< CGImageRef > image_from_bitmap( const BitMap& bitmap )
	{
		const short width  = bitmap.bounds.right - bitmap.bounds.left;
		const short height = bitmap.bounds.bottom - bitmap.bounds.top;
		const short stride = bitmap.rowBytes;
		
		return image_from_grayscale_data( width,
		                                  height,
		                                  1,  // weight
		                                  stride,
		                                  bitmap.baseAddr );
	}
	
	static
	n::owned< CGImageRef > image_from_pixmap( PixMapHandle pix )
	{
		const PixMap& pixmap = **pix;  // Blocks don't move in OS X
		
		const short width  = pixmap.bounds.right - pixmap.bounds.left;
		const short height = pixmap.bounds.bottom - pixmap.bounds.top;
		
		const long stride = get_pix_rowBytes( pix );
		
		n::saved< N::Pixels_State > saved_pixels_state( pix );
		
		const bool locked = ::LockPixels( pix );
		
		if ( ! locked )
		{
			return n::owned< CGImageRef >();
		}
		
		if ( const bool direct = pixmap.pixelType != 0 )
		{
			return image_from_RGB_data( width,
			                            height,
			                            pixmap.cmpSize,
			                            pixmap.pixelSize,
			                            stride,
			                            BitmapInfo_from_PixMap( pixmap ),
			                            pixmap.baseAddr );
		}
		
		if ( const bool monochrome = is_monochrome( pixmap ) )
		{
			return image_from_grayscale_data( width,
			                                  height,
			                                  pixmap.pixelSize,
			                                  stride,
			                                  pixmap.baseAddr );
		}
		
		// Indexed (non-gray, non-direct) pixmaps are not supported yet
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
				QuickDraw.  Create a new GWorld with the same depth and bounds
				and call CopyBits() to transcode the pixels for us.
			*/
			
			n::owned< GWorldPtr > native = N::NewGWorld( 16, pix[0]->bounds );
			
			PixMapHandle tmp = GetGWorldPixMap( native );
			
			if ( ::LockPixels( tmp ) &&  ::LockPixels( pix ) )
			{
				N::CopyBits( gworld, native );
				
				::UnlockPixels( pix );
				::UnlockPixels( tmp );
				
				return image_from_pixmap( tmp );
			}
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
