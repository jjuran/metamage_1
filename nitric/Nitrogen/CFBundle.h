// CFBundle.h

#ifndef NITROGEN_CFBUNDLE_H
#define NITROGEN_CFBUNDLE_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFBUNDLE__
#include FRAMEWORK_HEADER(CoreFoundation,CFBundle.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFARRAY_H
#include "Nitrogen/CFArray.h"
#endif
#ifndef NITROGEN_CFDICTIONARY_H
#include "Nitrogen/CFDictionary.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_CFURL_H
#include "Nitrogen/CFURL.h"
#endif

namespace Nitrogen
  {
   using ::CFBundleRef;
   using ::CFBundleGetTypeID;
   
   template <> struct CFType_Traits< CFBundleRef >: Basic_CFType_Traits< CFBundleRef, ::CFBundleGetTypeID > {};

   template <> struct OwnedDefaults< CFBundleRef >: OwnedDefaults< CFTypeRef >  {};
   
   class CFBundleGetMainBundle_Failed {};
   CFBundleRef CFBundleGetMainBundle();

   class CFBundleGetBundleWithIdentifier_Failed {};
   CFBundleRef CFBundleGetBundleWithIdentifier( CFStringRef bundleID );

   using ::CFBundleGetAllBundles;  // Is this really a "Get", not a "Copy"?
   
   class CFBundleCreate_Failed {};
   Owned<CFBundleRef> CFBundleCreate( CFAllocatorRef allocator, CFURLRef bundleURL );
   
   inline Owned<CFBundleRef> CFBundleCreate( CFURLRef bundleURL )
     {
      return Nitrogen::CFBundleCreate( kCFAllocatorDefault, bundleURL );
     }

   class CFBundleCreateBundlesFromDirectory_Failed {};
   Owned<CFArrayRef> CFBundleCreateBundlesFromDirectory( CFAllocatorRef allocator,
                                                         CFURLRef       directoryURL,
                                                         CFStringRef    bundleType );
   
   inline Owned<CFArrayRef> CFBundleCreateBundlesFromDirectory( CFURLRef    directoryURL,
                                                                CFStringRef bundleType )
     {
      return Nitrogen::CFBundleCreateBundlesFromDirectory( kCFAllocatorDefault, directoryURL, bundleType );
     }

   class CFBundleCopyBundleURL_Failed {};
   Owned<CFURLRef> CFBundleCopyBundleURL( CFBundleRef bundle );

   class CFBundleGetValueForInfoDictionaryKey_Failed {};
   CFTypeRef CFBundleGetValueForInfoDictionaryKey( CFBundleRef bundle,
                                                   CFStringRef key );

   class CFBundleGetInfoDictionary_Failed {};
   CFDictionaryRef CFBundleGetInfoDictionary( CFBundleRef bundle );

   class CFBundleGetLocalInfoDictionary_Failed {};
   CFDictionaryRef CFBundleGetLocalInfoDictionary( CFBundleRef bundle );

   /* ... */
   
   class CFBundleCopyResourceURL_Failed {};
   Owned<CFURLRef> CFBundleCopyResourceURL( CFBundleRef bundle,
                                            CFStringRef resourceName,
                                            CFStringRef resourceType,
                                            CFStringRef subDirName );
   
   /* ... */
   
   class CFBundleGetFunctionPointerForName_Failed {};
   void *CFBundleGetFunctionPointerForName( CFBundleRef bundle, CFStringRef functionName );

   template < class DesiredType >
   DesiredType CFBundleGetFunctionPointerForName( CFBundleRef bundle, CFStringRef functionName )
     {
      return reinterpret_cast< DesiredType >( Nitrogen::CFBundleGetFunctionPointerForName( bundle, functionName ) );
     }
  }

#endif
