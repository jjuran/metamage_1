// CFBundle.h

#ifndef NITROGEN_CFBUNDLE_H
#define NITROGEN_CFBUNDLE_H

#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

namespace Nitrogen
  {
   using ::CFBundleRef;
   
   template <> struct CFType_Traits< CFBundleRef >: Basic_CFType_Traits< CFBundleRef, ::CFBundleGetTypeID > {};

   template <> struct OwnedDefaults< CFBundleRef >: OwnedDefaults< CFTypeRef >  {};
   
   class CFBundleGetMainBundle_Failed {};
   CFBundleRef CFBundleGetMainBundle();

   class CFBundleGetBundleWithIdentifier_Failed {};
   CFBundleRef CFBundleGetBundleWithIdentifier( CFStringRef bundleID );

   class CFBundleCreate_Failed {};
   Owned<CFBundleRef> CFBundleCreate( CFAllocatorRef allocator, CFURLRef bundleURL );
   
   inline Owned<CFBundleRef> CFBundleCreate( CFURLRef bundleURL )
     {
      return Nitrogen::CFBundleCreate( kCFAllocatorDefault, bundleURL );
     }
   
   class CFBundleGetFunctionPointerForName_Failed {};
   void *CFBundleGetFunctionPointerForName( CFBundleRef bundle, CFStringRef functionName );

   template < class DesiredType >
   DesiredType CFBundleGetFunctionPointerForName( CFBundleRef bundle, CFStringRef functionName )
     {
      return reinterpret_cast< DesiredType >( Nitrogen::CFBundleGetFunctionPointerForName( bundle, functionName ) );
     }
  }

#endif
