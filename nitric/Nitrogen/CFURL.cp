// CFURL.cp

#ifndef NITROGEN_CFURL_H
#include "Nitrogen/CFURL.h"
#endif

namespace Nitrogen
  {
   Owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const UInt8 *      URLBytes,
                                         std::size_t        length,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL )
     {
      CFURLRef result = ::CFURLCreateWithBytes( allocator,
                                                URLBytes,
                                                static_cast<CFIndex>( length ),
                                                encoding,
                                                baseURL );
      if ( result == 0 )
         throw CFURLCreateWithBytes_Failed();
      return Owned<CFURLRef>::Seize( result );
     }

   Owned<CFURLRef> CFURLCreateWithBytes( CFAllocatorRef     allocator,
                                         const std::string& string,
                                         CFStringEncoding   encoding,
                                         CFURLRef           baseURL )
     {
      return CFURLCreateWithBytes( allocator,
                                   reinterpret_cast<const UInt8 *>( string.data() ),
                                   string.size(),
                                   encoding,
                                   baseURL );
     }

   Owned<CFURLRef> CFURLCreateWithString( CFAllocatorRef   allocator,
                                          CFStringRef      URLString,
                                          CFURLRef         baseURL )
     {
      CFURLRef result = ::CFURLCreateWithString( allocator, URLString, baseURL );
      if ( result == 0 )
         throw CFURLCreateWithString_Failed();
      return Owned<CFURLRef>::Seize( result );
     }

   Owned<CFURLRef> CFURLCreateFromFSRef( CFAllocatorRef   allocator,
                                         const FSRef&     fsRef )
     {
      CFURLRef result = ::CFURLCreateFromFSRef( allocator, &fsRef );
      if ( result == 0 )
         throw CFURLCreateFromFSRef_Failed();
      return Owned<CFURLRef>::Seize( result );
     }
  }
