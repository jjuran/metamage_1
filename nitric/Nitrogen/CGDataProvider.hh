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

namespace nucleus {
   template <> struct disposer< CGDataProviderRef >
     {
      typedef CGDataProviderRef  argument_type;
      typedef void               result_type;
      
      void operator()( CGDataProviderRef image ) const
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
	inline nucleus::owned<CGDataProviderRef> CGDataProviderCreateWithCFData ( CFDataRef data ) {
		CGDataProviderRef result = ::CGDataProviderCreateWithCFData ( data );
		if ( NULL == result ) throw CGDataProviderCreateWithCFData_Failed ();
		return nucleus::owned< CGDataProviderRef>::seize ( result );
		}
  
#endif
	
	class CGDataProviderCreateWithURL_Failed {};
	inline nucleus::owned<CGDataProviderRef> CGDataProviderCreateWithURL ( CFURLRef url ) {
		CGDataProviderRef result = ::CGDataProviderCreateWithURL ( url );
		if ( NULL == result ) throw CGDataProviderCreateWithURL_Failed ();
		return nucleus::owned< CGDataProviderRef>::seize ( result );
		}
	
   inline nucleus::owned<CGDataProviderRef> CGDataProviderRetain( CGDataProviderRef image )
     {
      return nucleus::owned<CGDataProviderRef>::seize( ::CGDataProviderRetain( image ) );
     }
   
   inline void CGDataProviderRelease( nucleus::owned<CGDataProviderRef> )
     {}

  }

#endif
