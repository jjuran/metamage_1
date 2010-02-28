// Nitrogen/CGDataProvider.hh
// --------------------------

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow, Lisa Lippincott, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CGDATAPROVIDER_HH
#define NITROGEN_CGDATAPROVIDER_HH

#ifndef CGDATAPROVIDER_H_
#include <CGDataProvider.h>
#endif
#ifndef NITROGEN_CGBASE_HH
#include "Nitrogen/CGBase.hh"
#endif
#ifndef NITROGEN_CFURL_HH
#include "Nitrogen/CFURL.hh"
#endif

namespace Nitrogen
  {
   using ::CGDataProviderRef; 
  }

namespace Nucleus {
   template <> struct Disposer< Nitrogen::CGDataProviderRef >: public std::unary_function< Nitrogen::CGDataProviderRef, void >
     {
      void operator()( Nitrogen::CGDataProviderRef image ) const
        {
         ::CGDataProviderRelease ( image );
        }
     };
  }

namespace Nitrogen
  {
#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
  
  using ::CGDataProviderGetTypeID;
  
//	CG_EXTERN CGDataProviderRef CGDataProviderCreate(void *info, const CGDataProviderCallbacks *callbacks);
//  CG_EXTERN CGDataProviderRef CGDataProviderCreateDirectAccess(void *info, size_t size, const CGDataProviderDirectAccessCallbacks *callbacks);
//	CG_EXTERN CGDataProviderRef CGDataProviderCreateWithData(void *info, const void *data, size_t size, CGDataProviderReleaseDataCallback releaseData);

	class CGDataProviderCreateWithCFData_Failed {};
	inline Nucleus::Owned<CGDataProviderRef> CGDataProviderCreateWithCFData ( CFDataRef data ) {
		CGDataProviderRef result = ::CGDataProviderCreateWithCFData ( data );
		if ( NULL == result ) throw CGDataProviderCreateWithCFData_Failed ();
		return Nucleus::Owned< CGDataProviderRef>::Seize ( result );
		}
  
#endif
	
	class CGDataProviderCreateWithURL_Failed {};
	inline Nucleus::Owned<CGDataProviderRef> CGDataProviderCreateWithURL ( CFURLRef url ) {
		CGDataProviderRef result = ::CGDataProviderCreateWithURL ( url );
		if ( NULL == result ) throw CGDataProviderCreateWithURL_Failed ();
		return Nucleus::Owned< CGDataProviderRef>::Seize ( result );
		}
	
   inline Nucleus::Owned<CGDataProviderRef> CGDataProviderRetain( CGDataProviderRef image )
     {
      return Nucleus::Owned<CGDataProviderRef>::Seize( ::CGDataProviderRetain( image ) );
     }
   
   inline void CGDataProviderRelease( Nucleus::Owned<CGDataProviderRef> )
     {}

  }

#endif
