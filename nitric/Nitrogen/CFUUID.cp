// CFUUID.cp

#ifndef NITROGEN_CFUUID_H
#include "Nitrogen/CFUUID.h"
#endif

namespace Nitrogen
  {
   Owned< CFUUIDRef > CFUUIDCreate( CFAllocatorRef alloc )
     {
      CFUUIDRef result = ::CFUUIDCreate( alloc );
      if ( result == 0 )
         throw CFUUIDCreate_Failed();
      return Owned< CFUUIDRef >::Seize( result );
     }

   Owned< CFUUIDRef > CFUUIDCreateWithBytes( CFAllocatorRef alloc,
                                             UInt8          byte0,
                                             UInt8          byte1,
                                             UInt8          byte2,
                                             UInt8          byte3,
                                             UInt8          byte4,
                                             UInt8          byte5,
                                             UInt8          byte6,
                                             UInt8          byte7,
                                             UInt8          byte8,
                                             UInt8          byte9,
                                             UInt8          byte10,
                                             UInt8          byte11,
                                             UInt8          byte12,
                                             UInt8          byte13,
                                             UInt8          byte14,
                                             UInt8          byte15 )
     {
      CFUUIDRef result = ::CFUUIDCreateWithBytes( alloc,
                                                  byte0,  byte1,  byte2,  byte3,
                                                  byte4,  byte5,  byte6,  byte7,
                                                  byte8,  byte9,  byte10, byte11,
                                                  byte12, byte13, byte14, byte15 );
      if ( result == 0 )
         throw CFUUIDCreateWithBytes_Failed();
      return Owned< CFUUIDRef >::Seize( result );
     }

   Owned< CFUUIDRef > CFUUIDCreateFromString( CFAllocatorRef alloc,
                                              CFStringRef    uuidStr )
     {
      CFUUIDRef result = ::CFUUIDCreateFromString( alloc, uuidStr );
      if ( result == 0 )
         throw CFUUIDCreateFromString_Failed();
      return Owned< CFUUIDRef >::Seize( result );
     }

   Owned< CFStringRef > CFUUIDCreateString( CFAllocatorRef alloc,
                                            CFUUIDRef      uuid )
     {
      CFStringRef result = ::CFUUIDCreateString( alloc, uuid );
      if ( result == 0 )
         throw CFUUIDCreateString_Failed();
      return Owned< CFStringRef >::Seize( result );
     }

   CFUUIDRef CFUUIDGetConstantUUIDWithBytes( CFAllocatorRef alloc,
                                             UInt8          byte0,
                                             UInt8          byte1,
                                             UInt8          byte2,
                                             UInt8          byte3,
                                             UInt8          byte4,
                                             UInt8          byte5,
                                             UInt8          byte6,
                                             UInt8          byte7,
                                             UInt8          byte8,
                                             UInt8          byte9,
                                             UInt8          byte10,
                                             UInt8          byte11,
                                             UInt8          byte12,
                                             UInt8          byte13,
                                             UInt8          byte14,
                                             UInt8          byte15 )
     {
      CFUUIDRef result = ::CFUUIDGetConstantUUIDWithBytes( alloc,
                                                           byte0,  byte1,  byte2,  byte3,
                                                           byte4,  byte5,  byte6,  byte7,
                                                           byte8,  byte9,  byte10, byte11,
                                                           byte12, byte13, byte14, byte15 );
      if ( result == 0 )
         throw CFUUIDGetConstantUUIDWithBytes_Failed();
      return result;
     }

   Owned<CFUUIDRef> CFUUIDCreateFromUUIDBytes( CFAllocatorRef alloc,
                                               CFUUIDBytes    bytes )
     {
      CFUUIDRef result = ::CFUUIDCreateFromUUIDBytes( alloc, bytes );
      if ( result == 0 )
         throw CFUUIDCreateFromUUIDBytes_Failed();
      return Owned< CFUUIDRef >::Seize( result );
     }
  }
