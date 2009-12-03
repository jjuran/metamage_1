// Nitrogen/CGImage.h
// ------------------

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow, Lisa Lippincott, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CGIMAGE_H
#define NITROGEN_CGIMAGE_H

#ifndef CGIMAGE_H_
#include <CGImage.h>
#endif
#ifndef NITROGEN_CGBASE_H
#include "Nitrogen/CGBase.h"
#endif
#ifndef NITROGEN_CGCOLORSPACE_H
#include "Nitrogen/CGColorSpace.h"
#endif
#ifndef NITROGEN_CGDATAPROVIDER_H
#include "Nitrogen/CGDataProvider.h"
#endif

namespace Nitrogen
  {
   using ::CGImageRef;
  }

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::CGImageRef >: public std::unary_function< Nitrogen::CGImageRef, void >
     {
      void operator()( Nitrogen::CGImageRef image ) const
        {
         ::CGImageRelease( image );
        }
     };
  }

namespace Nitrogen
  {
   inline Nucleus::Owned<CGImageRef> CGImageRetain( CGImageRef image )
     {
      return Nucleus::Owned<CGImageRef>::Seize( ::CGImageRetain( image ) );
     }
   
   inline void CGImageRelease( Nucleus::Owned<CGImageRef> )
     {}
	 
#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
	
	using ::CGImageGetTypeID;	// CFTypeID CGImageGetTypeID(void)
	

	class CGImageCreate_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageCreate (
				size_t width, size_t height, size_t bitsPerComponent, size_t bitsPerPixel, size_t bytesPerRow, 
				CGColorSpaceRef colorspace, CGBitmapInfo bitmapInfo, CGDataProviderRef provider, 
				const float decode[] = NULL, bool shouldInterpolate = false, CGColorRenderingIntent intent = kCGRenderingIntentDefault ) {
		CGImageRef result = ::CGImageCreate ( width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, 
										colorspace, bitmapInfo, provider, decode, shouldInterpolate, intent );
		if ( NULL == result ) throw CGImageCreate_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
		}
	
#endif
	
	class CGImageMaskCreate_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageMaskCreate (
				size_t width, size_t height, size_t bitsPerComponent, size_t bitsPerPixel, size_t bytesPerRow, 
				CGDataProviderRef provider, const float decode[] = NULL, bool shouldInterpolate = false) {
		CGImageRef result = ::CGImageMaskCreate ( width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, 
										provider, decode, shouldInterpolate );
		if ( NULL == result ) throw CGImageMaskCreate_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
		}

#if UNIVERSAL_INTERFACES_VERSION >= 0x0400

	class CGImageCreateCopy_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageCreateCopy ( CGImageRef image ) {
		CGImageRef result = ::CGImageCreateCopy ( image ); 
		if ( NULL == result ) throw CGImageCreateCopy_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
		}
	
#endif

	class CGImageCreateWithJPEGDataProvider_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageCreateWithJPEGDataProvider (
				CGDataProviderRef source, const float decode[] = NULL,
				bool shouldInterpolate = false, CGColorRenderingIntent intent = kCGRenderingIntentDefault ) {
		CGImageRef result = ::CGImageCreateWithJPEGDataProvider ( source, decode, shouldInterpolate, intent );
		if ( NULL == result ) throw CGImageCreateWithJPEGDataProvider_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
		}

#if UNIVERSAL_INTERFACES_VERSION >= 0x0400

	class CGImageCreateWithPNGDataProvider_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageCreateWithPNGDataProvider (
				CGDataProviderRef source, const float decode[] = NULL,
				bool shouldInterpolate = false, CGColorRenderingIntent intent = kCGRenderingIntentDefault ) {
		CGImageRef result = ::CGImageCreateWithPNGDataProvider ( source, decode, shouldInterpolate, intent );
		if ( NULL == result ) throw CGImageCreateWithPNGDataProvider_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
		}
	
	class CGImageCreateWithImageInRect_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageCreateWithImageInRect ( CGImageRef image, CGRect rect ) {
		CGImageRef result = ::CGImageCreateWithImageInRect ( image, rect );
		if ( NULL == result ) throw CGImageCreateWithImageInRect_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
		}


	class CGImageCreateWithMask_Failed {};
	inline Nucleus::Owned<CGImageRef> CGImageCreateWithMask ( CGImageRef image, CGImageRef mask ) {
		CGImageRef result = ::CGImageCreateWithMask ( image, mask );
		if ( NULL == result ) throw CGImageCreateWithMask_Failed ();
		return Nucleus::Owned<CGImageRef>::Seize ( result );
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
