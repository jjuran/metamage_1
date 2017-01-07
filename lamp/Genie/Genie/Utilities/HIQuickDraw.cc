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
	bool is_grayscale( const PixMap& pixmap )
	{
		if ( CTabHandle ctab = pixmap.pmTable )
		{
			const long ctSeed = ctab[0]->ctSeed;
			
			return ctSeed >= 32 + 1  &&  ctSeed <= 32 + 8;
		}
		
		return false;
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
	
	static
	n::owned< CGImageRef > image_from_RGB_data( size_t  width,
	                                            size_t  height,
	                                            size_t  degree,
	                                            size_t  weight,
	                                            size_t  stride,
	                                            char*   baseAddr )
	{
		CGBitmapInfo bitmapInfo = kCGImageAlphaNoneSkipFirst;
		
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
		const short stride = pixmap.rowBytes & 0x3FFF;
		
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
			                            pixmap.baseAddr );
		}
		
		if ( const bool grayscale = is_grayscale( pixmap ) )
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
		HIViewDrawPixMap( context,
		                  bounds,
		                  GetGWorldPixMap( gworld ) );
	}
	
}
