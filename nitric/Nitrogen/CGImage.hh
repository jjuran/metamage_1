// Nitrogen/CGImage.hh
// -------------------

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow, Lisa Lippincott, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CGIMAGE_HH
#define NITROGEN_CGIMAGE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGIMAGE_H_
#ifndef __CGIMAGE__
#include <CGImage.h>
#endif
#endif

// Nitrogen
#ifndef NITROGEN_CGDATAPROVIDER_HH
#include "Nitrogen/CGDataProvider.hh"
#endif


namespace nucleus
{
	
	template <> struct disposer< CGImageRef >
	{
		typedef CGImageRef  argument_type;
		typedef void        result_type;
		
		void operator()( CGImageRef image ) const
		{
			::CGImageRelease( image );
		}
	};
	
}

namespace Nitrogen
{
	
#ifndef MAC_OS_X_VERSION_10_4
	
	// This definition of CGBitmapInfo should be regarded as private.
	typedef CGImageAlphaInfo CGBitmapInfo;
	
#endif
	
#ifndef MAC_OS_X_VERSION_10_5
	
	typedef float CGFloat;
	
#endif
	
	inline
	nucleus::owned< CGImageRef > CGImageRetain( CGImageRef image )
	{
		return nucleus::owned< CGImageRef >::seize( ::CGImageRetain( image ) );
	}
	
	inline
	void CGImageRelease( nucleus::owned< CGImageRef > )
	{
	}
	
#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
	
	using ::CGImageGetTypeID;  // CFTypeID CGImageGetTypeID(void)
	
#endif
	
	
	class CGImageCreate_Failed {};
	
	inline
	nucleus::owned< CGImageRef >
	//
	CGImageCreate( size_t                  width,
	               size_t                  height,
	               size_t                  bitsPerComponent,
	               size_t                  bitsPerPixel,
	               size_t                  bytesPerRow,
	               CGColorSpaceRef         colorspace,
	               CGBitmapInfo            bitmapInfo,
	               CGDataProviderRef       provider,
	               const CGFloat           decode[]          = NULL,
	               bool                    shouldInterpolate = false,
	               CGColorRenderingIntent  intent            = kCGRenderingIntentDefault )
	{
		CGImageRef result = ::CGImageCreate( width,
		                                     height,
		                                     bitsPerComponent,
		                                     bitsPerPixel,
		                                     bytesPerRow,
		                                     colorspace,
		                                     bitmapInfo,
		                                     provider,
		                                     decode,
		                                     shouldInterpolate,
		                                     intent );
		
		if ( result == NULL )
		{
			throw CGImageCreate_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
	class CGImageMaskCreate_Failed {};
	
	inline
	nucleus::owned< CGImageRef >
	//
	CGImageMaskCreate( size_t             width,
	                   size_t             height,
	                   size_t             bitsPerComponent,
	                   size_t             bitsPerPixel,
	                   size_t             bytesPerRow,
	                   CGDataProviderRef  provider,
	                   const CGFloat      decode[]          = NULL,
	                   bool               shouldInterpolate = false )
	{
		CGImageRef result = ::CGImageMaskCreate( width,
		                                         height,
		                                         bitsPerComponent,
		                                         bitsPerPixel,
		                                         bytesPerRow,
		                                         provider,
		                                         decode,
		                                         shouldInterpolate );
		
		if ( result == NULL )
		{
			throw CGImageMaskCreate_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
#ifdef MAC_OS_X_VERSION_10_4
	
	class CGImageCreateCopy_Failed {};
	
	inline
	nucleus::owned< CGImageRef > CGImageCreateCopy( CGImageRef image )
	{
		CGImageRef result = ::CGImageCreateCopy( image );
		
		if ( result == NULL )
		{
			throw CGImageCreateCopy_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
#endif
	
	class CGImageCreateWithJPEGDataProvider_Failed {};
	
	inline
	nucleus::owned< CGImageRef >
	//
	CGImageCreateWithJPEGDataProvider( CGDataProviderRef       source,
	                                   const CGFloat           decode[]          = NULL,
	                                   bool                    shouldInterpolate = false,
	                                   CGColorRenderingIntent  intent            = kCGRenderingIntentDefault )
	{
		CGImageRef result = ::CGImageCreateWithJPEGDataProvider( source,
		                                                         decode,
		                                                         shouldInterpolate,
		                                                         intent );
		
		if ( result == NULL )
		{
			throw CGImageCreateWithJPEGDataProvider_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
	
	class CGImageCreateWithPNGDataProvider_Failed {};
	
	inline
	nucleus::owned< CGImageRef >
	//
	CGImageCreateWithPNGDataProvider( CGDataProviderRef       source,
	                                  const CGFloat           decode[]          = NULL,
	                                  bool                    shouldInterpolate = false,
	                                  CGColorRenderingIntent  intent            = kCGRenderingIntentDefault )
	{
		CGImageRef result = ::CGImageCreateWithPNGDataProvider( source,
		                                                        decode,
		                                                        shouldInterpolate,
		                                                        intent );
		
		if ( result == NULL )
		{
			throw CGImageCreateWithPNGDataProvider_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
#ifdef MAC_OS_X_VERSION_10_4
	
	class CGImageCreateWithImageInRect_Failed {};
	
	inline
	nucleus::owned< CGImageRef >
	//
	CGImageCreateWithImageInRect( CGImageRef image, CGRect rect )
	{
		CGImageRef result = ::CGImageCreateWithImageInRect( image, rect );
		
		if ( result == NULL )
		{
			throw CGImageCreateWithImageInRect_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
	
	class CGImageCreateWithMask_Failed {};
	
	inline
	nucleus::owned< CGImageRef >
	//
	CGImageCreateWithMask( CGImageRef image, CGImageRef mask )
	{
		CGImageRef result = ::CGImageCreateWithMask( image, mask );
		
		if ( result == NULL )
		{
			throw CGImageCreateWithMask_Failed();
		}
		
		return nucleus::owned< CGImageRef >::seize( result );
	}
	
#endif
#endif
	
//	CG_EXTERN CGImageRef CGImageCreateWithMaskingColors(CGImageRef image, const float components[]) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
//	CG_EXTERN CGImageRef CGImageCreateCopyWithColorSpace(CGImageRef image, CGColorSpaceRef colorspace) AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;
	
	using ::CGImageIsMask;
	using ::CGImageGetWidth;
	using ::CGImageGetHeight;
	using ::CGImageGetBitsPerComponent;
	using ::CGImageGetBitsPerPixel;
	using ::CGImageGetBytesPerRow;
	
//	CG_EXTERN CGColorSpaceRef CGImageGetColorSpace(CGImageRef image);
	using ::CGImageGetAlphaInfo;
	using ::CGImageGetDataProvider;
//	const float *CGImageGetDecode(CGImageRef image);
	using ::CGImageGetShouldInterpolate;
	using ::CGImageGetRenderingIntent;
	
#ifdef MAC_OS_X_VERSION_10_4

	using ::CGImageGetBitmapInfo;
	
#endif
	
}

#endif
