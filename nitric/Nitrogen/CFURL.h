// CFURL.h

#ifndef NITROGEN_CFURL_H
#define NITROGEN_CFURL_H

#ifndef __CFURL__
#include <CFURL.h>
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

#include <string>

namespace Nitrogen
  {
   using ::CFURLRef;
   
   template <> struct CFType_Traits< CFURLRef >: Basic_CFType_Traits< CFURLRef, ::CFURLGetTypeID > {};

   template <> struct OwnedDefaults< CFURLRef >: OwnedDefaults< CFTypeRef >  {};

   class CFURLCreateWithBytes_Failed {};
   Owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const UInt8 *      URLBytes,
                                         std::size_t        length,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL = 0 );

   inline Owned<CFURLRef> CFURLCreateWithBytes( const UInt8 *      URLBytes,
                                                std::size_t        length,
                                                CFStringEncoding   encoding,
                                                CFURLRef           baseURL = 0 )
     {
      return Nitrogen::CFURLCreateWithBytes( kCFAllocatorDefault, URLBytes, length, encoding, baseURL );
     }

   Owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const std::string& URLBytes,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL = 0 );

   inline Owned<CFURLRef> CFURLCreateWithBytes( const std::string& URLBytes,
                                                CFStringEncoding   encoding,
                                                CFURLRef           baseURL = 0 )
     {
      return Nitrogen::CFURLCreateWithBytes( kCFAllocatorDefault, URLBytes, encoding, baseURL );
     }

   class CFURLCreateWithString_Failed {};
   Owned<CFURLRef> CFURLCreateWithString( CFAllocatorRef   allocator,
                                          CFStringRef      URLString,
                                          CFURLRef         baseURL = 0 );

   inline Owned<CFURLRef> CFURLCreateWithString( CFStringRef      URLString,
                                                 CFURLRef         baseURL = 0 )
     {
      return Nitrogen::CFURLCreateWithString( kCFAllocatorDefault, URLString, baseURL );
     }

   class CFURLCreateFromFSRef_Failed {};
   Owned<CFURLRef> CFURLCreateFromFSRef( CFAllocatorRef   allocator,
                                         const FSRef&     fsRef );
   
   inline Owned<CFURLRef> CFURLCreateFromFSRef( const FSRef& fsRef )
     {
      return Nitrogen::CFURLCreateFromFSRef( kCFAllocatorDefault, fsRef );
     }
  }

#endif
