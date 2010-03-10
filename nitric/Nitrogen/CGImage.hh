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

#ifndef CGIMAGE_H_
#include <CGImage.h>
#endif
#ifndef NITROGEN_CGBASE_HH
#include "Nitrogen/CGBase.hh"
#endif
#ifndef NITROGEN_CGCOLORSPACE_HH
#include "Nitrogen/CGColorSpace.hh"
#endif
#ifndef NITROGEN_CGDATAPROVIDER_HH
#include "Nitrogen/CGDataProvider.hh"
#endif

namespace Nitrogen
  {
   using ::CGImageRef;
  }

namespace nucleus
  {
   template <> struct disposer< Nitrogen::CGImageRef >: public std::unary_function< Nitrogen::CGImageRef, void >
     {
      void operator()( Nitrogen::CGImageRef image ) const
        {
         ::CGImageRelease( image );
        }
     };
  }

namespace Nitrogen
  {
   inline nucleus::owned<CGImageRef> CGImageRetain( CGImageRef image )
     {
      return nucleus::owned<CGImageRef>::seize( ::CGImageRetain( image ) );
     }
   
   inline void CGImageRelease( nucleus::owned<CGImageRef> )
     {}
	 
#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
	
	using ::CGImageGetTypeID;	// CFTypeID CGImageGetTypeID(void)
	

	class CGImageCreate_Failed {};
	inline nucleus::owned<CGImageRef> CGImageCreate (
				size_t width, size_t height, size_t bitsPerComponent, size_t bitsPerPixel, size_t bytesPerRow, 
				CGColorSpaceRef colorspace, CGBitmapInfo bitmapInfo, CGDataProviderRef provider, 
				const float decode[] = NULL, bool shouldInterpolate = false, CGColorRenderingIntent intent = kCGRenderingIntentDefault ) {
		CGImageRef result = ::CGImageCreate ( width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, 
										colorspace, bitmapInfo, provider, decode, shouldInterpolate, intent );
		if ( NULL == result ) throw CGImageCreate_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}
	
#endif
	
	class CGImageMaskCreate_Failed {};
	inline nucleus::owned<CGImageRef> CGImageMaskCreate (
				size_t width, size_t height, size_t bitsPerComponent, size_t bitsPerPixel, size_t bytesPerRow, 
				CGDataProviderRef provider, const float decode[] = NULL, bool shouldInterpolate = false) {
		CGImageRef result = ::CGImageMaskCreate ( width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, 
										provider, decode, shouldInterpolate );
		if ( NULL == result ) throw CGImageMaskCreate_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}

#if UNIVERSAL_INTERFACES_VERSION >= 0x0400

	class CGImageCreateCopy_Failed {};
	inline nucleus::owned<CGImageRef> CGImageCreateCopy ( CGImageRef image ) {
		CGImageRef result = ::CGImageCreateCopy ( image ); 
		if ( NULL == result ) throw CGImageCreateCopy_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}
	
#endif

	class CGImageCreateWithJPEGDataProvider_Failed {};
	inline nucleus::owned<CGImageRef> CGImageCreateWithJPEGDataProvider (
				CGDataProviderRef source, const float decode[] = NULL,
				bool shouldInterpolate = false, CGColorRenderingIntent intent = kCGRenderingIntentDefault ) {
		CGImageRef result = ::CGImageCreateWithJPEGDataProvider ( source, decode, shouldInterpolate, intent );
		if ( NULL == result ) throw CGImageCreateWithJPEGDataProvider_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}

#if UNIVERSAL_INTERFACES_VERSION >= 0x0400

	class CGImageCreateWithPNGDataProvider_Failed {};
	inline nucleus::owned<CGImageRef> CGImageCreateWithPNGDataProvider (
				CGDataProviderRef source, const float decode[] = NULL,
				bool shouldInterpolate = false, CGColorRenderingIntent intent = kCGRenderingIntentDefault ) {
		CGImageRef result = ::CGImageCreateWithPNGDataProvider ( source, decode, shouldInterpolate, intent );
		if ( NULL == result ) throw CGImageCreateWithPNGDataProvider_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}
	
	class CGImageCreateWithImageInRect_Failed {};
	inline nucleus::owned<CGImageRef> CGImageCreateWithImageInRect ( CGImageRef image, CGRect rect ) {
		CGImageRef result = ::CGImageCreateWithImageInRect ( image, rect );
		if ( NULL == result ) throw CGImageCreateWithImageInRect_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}


	class CGImageCreateWithMask_Failed {};
	inline nucleus::owned<CGImageRef> CGImageCreateWithMask ( CGImageRef image, CGImageRef mask ) {
		CGImageRef result = ::CGImageCreateWithMask ( image, mask );
		if ( NULL == result ) throw CGImageCreateWithMask_Failed ();
		return nucleus::owned<CGImageRef>::seize ( result );
		}

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

#if UNIVERSAL_INTERFACES_VERSION >= 0x0400

	using ::CGImageGetBitmapInfo;
	
#endif
  }

#endif
