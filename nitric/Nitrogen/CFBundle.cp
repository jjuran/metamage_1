// CFBundle.cp

#ifndef NITROGEN_CFBUNDLE_H
#include "Nitrogen/CFBundle.h"
#endif

namespace Nitrogen
  {
   CFBundleRef CFBundleGetMainBundle()
     {
      CFBundleRef result = ::CFBundleGetMainBundle();
      if ( result == 0 )
         throw CFBundleGetMainBundle_Failed();
      return result;
     }

   CFBundleRef CFBundleGetBundleWithIdentifier( CFStringRef bundleID )
     {
      CFBundleRef result = ::CFBundleGetBundleWithIdentifier( bundleID );
      if ( result == 0 )
         throw CFBundleGetBundleWithIdentifier_Failed();
      return result;
     }

   Owned<CFBundleRef> CFBundleCreate( CFAllocatorRef allocator, CFURLRef bundleURL )
     {
      CFBundleRef result = ::CFBundleCreate( allocator, bundleURL );
      if ( result == 0 )
         throw CFBundleCreate_Failed();
      return Owned<CFBundleRef>::Seize( result );
     }

   void *CFBundleGetFunctionPointerForName( CFBundleRef bundle, CFStringRef functionName )
     {
      void *result = ::CFBundleGetFunctionPointerForName( bundle, functionName );
      if ( result == 0 )
         throw CFBundleGetFunctionPointerForName_Failed();
      return result;
     }
  }
