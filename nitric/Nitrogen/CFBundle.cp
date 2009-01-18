// Nitrogen/CFBundle.cp
// --------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/CFBundle.h"


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

   Nucleus::Owned<CFBundleRef> CFBundleCreate( CFAllocatorRef allocator, CFURLRef bundleURL )
     {
      CFBundleRef result = ::CFBundleCreate( allocator, bundleURL );
      if ( result == 0 )
         throw CFBundleCreate_Failed();
      return Nucleus::Owned<CFBundleRef>::Seize( result );
     }

   Nucleus::Owned<CFArrayRef> CFBundleCreateBundlesFromDirectory( CFAllocatorRef allocator,
                                                         CFURLRef       directoryURL,
                                                         CFStringRef    bundleType )
     {
      CFArrayRef result = ::CFBundleCreateBundlesFromDirectory( allocator, directoryURL, bundleType );
      if ( result == 0 )
         throw CFBundleCreateBundlesFromDirectory_Failed();
      return Nucleus::Owned<CFArrayRef>::Seize( result );
     }

   Nucleus::Owned<CFURLRef> CFBundleCopyBundleURL( CFBundleRef bundle )
     {
      CFURLRef result = ::CFBundleCopyBundleURL( bundle );
      if ( result == 0 )
         throw CFBundleCopyBundleURL_Failed();
      return Nucleus::Owned<CFURLRef>::Seize( result );
     }

   CFTypeRef CFBundleGetValueForInfoDictionaryKey( CFBundleRef bundle,
                                                   CFStringRef key )
     {
      CFTypeRef result = ::CFBundleGetValueForInfoDictionaryKey( bundle, key );
      if ( result == 0 )
         throw CFBundleGetValueForInfoDictionaryKey_Failed();
      return result;
     }

   CFDictionaryRef CFBundleGetInfoDictionary( CFBundleRef bundle )
     {
      CFDictionaryRef result = ::CFBundleGetInfoDictionary( bundle );
      if ( result == 0 )
         throw CFBundleGetInfoDictionary_Failed();
      return result;
     }

   CFDictionaryRef CFBundleGetLocalInfoDictionary( CFBundleRef bundle )
     {
      CFDictionaryRef result = ::CFBundleGetLocalInfoDictionary( bundle );
      if ( result == 0 )
         throw CFBundleGetLocalInfoDictionary_Failed();
      return result;
     }

   Nucleus::Owned<CFURLRef> CFBundleCopyResourceURL( CFBundleRef bundle,
                                            CFStringRef resourceName,
                                            CFStringRef resourceType,
                                            CFStringRef subDirName )
     {
      CFURLRef result = ::CFBundleCopyResourceURL( bundle, resourceName, resourceType, subDirName );
      if ( result == 0 )
         throw CFBundleCopyResourceURL_Failed();
      return Nucleus::Owned<CFURLRef>::Seize( result );
     }

   void *CFBundleGetFunctionPointerForName( CFBundleRef bundle, CFStringRef functionName )
     {
      void *result = ::CFBundleGetFunctionPointerForName( bundle, functionName );
      if ( result == 0 )
         throw CFBundleGetFunctionPointerForName_Failed();
      return result;
     }
  }
