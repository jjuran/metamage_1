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

// Nitrogen
#include "Nitrogen/CGColorSpace.hh"
#include "Nitrogen/CGDataProvider.hh"
#include "Nitrogen/CGImage.hh"


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
	
	static
	OSStatus HIViewDrawCGImageUninterpolated( CGContextRef  context,
	                                          CGRect        bounds,
	                                          CGImageRef    image )
	{
		OSStatus err = 0;
		
		CGContextSaveGState( context );
		
		CGContextSetInterpolationQuality( context, kCGInterpolationNone );
		
	#ifdef MAC_OS_X_VERSION_10_2
		
		err = HIViewDrawCGImage( context, &bounds, image );
		
	#endif
		
		CGContextRestoreGState( context );
		
		return err;
	}
	
	static
	n::owned< CGImageRef > image_from_data( size_t           width,
	                                        size_t           height,
	                                        size_t           degree,
	                                        size_t           weight,
	                                        size_t           stride,
	                                        CGColorSpaceRef  colorSpace,
	                                        char*            baseAddr,
	                                        copier           cpy )
	{
		return N::CGImageCreate( width,
		                         height,
		                         degree,  // bits per component
		                         weight,  // bits per pixel
		                         stride,
		                         colorSpace,
		                         kCGImageAlphaNoneSkipFirst,
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
		                        baseAddr,
		                        &inverted_copy );
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
	
	void HIViewDrawBitMap( CGContextRef   context,
	                       CGRect         bounds,
	                       const BitMap&  bitmap )
	{
		OSStatus err;
		
		err = HIViewDrawCGImageUninterpolated( context,
		                                       bounds,
		                                       image_from_bitmap( bitmap ) );
	}
	
}
