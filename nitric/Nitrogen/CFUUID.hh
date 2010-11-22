// Nitrogen/CFUUID.hh
// ------------------

// Part of the Nitrogen project.
//
// Written 2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFUUID_HH
#define NITROGEN_CFUUID_HH

#ifndef __CFUUID__
#include <CFUUID.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

namespace Nitrogen
  {
   using ::CFUUIDRef;
  }

namespace nucleus
  {
   template <> struct disposer_class< CFUUIDRef >: disposer_class< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {
   template <> struct CFType_Traits< CFUUIDRef >: Basic_CFType_Traits< CFUUIDRef, ::CFUUIDGetTypeID > {};

   using ::CFUUIDBytes;
   
   using ::CFUUIDGetTypeID;
   
   class CFUUIDCreate_Failed {};
   nucleus::owned< CFUUIDRef > CFUUIDCreate( CFAllocatorRef alloc = kCFAllocatorDefault );

   class CFUUIDCreateWithBytes_Failed {};
   nucleus::owned< CFUUIDRef > CFUUIDCreateWithBytes( CFAllocatorRef alloc,
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
                                                      UInt8          byte15 );
   
   inline nucleus::owned< CFUUIDRef > CFUUIDCreateWithBytes( UInt8 byte0,
                                                             UInt8 byte1,
                                                             UInt8 byte2,
                                                             UInt8 byte3,
                                                             UInt8 byte4,
                                                             UInt8 byte5,
                                                             UInt8 byte6,
                                                             UInt8 byte7,
                                                             UInt8 byte8,
                                                             UInt8 byte9,
                                                             UInt8 byte10,
                                                             UInt8 byte11,
                                                             UInt8 byte12,
                                                             UInt8 byte13,
                                                             UInt8 byte14,
                                                             UInt8 byte15 )
     {
      return Nitrogen::CFUUIDCreateWithBytes( kCFAllocatorDefault,
                                              byte0,  byte1,  byte2,  byte3,
                                              byte4,  byte5,  byte6,  byte7,
                                              byte8,  byte9,  byte10, byte11,
                                              byte12, byte13, byte14, byte15 );
     }

   class CFUUIDCreateFromString_Failed {};
   nucleus::owned< CFUUIDRef > CFUUIDCreateFromString( CFAllocatorRef alloc,
                                                       CFStringRef    uuidStr );
   
   inline nucleus::owned< CFUUIDRef > CFUUIDCreateFromString( CFStringRef uuidStr )
     {
      return Nitrogen::CFUUIDCreateFromString( kCFAllocatorDefault, uuidStr );
     }

   class CFUUIDCreateString_Failed {};
   nucleus::owned< CFStringRef > CFUUIDCreateString( CFAllocatorRef alloc,
                                                     CFUUIDRef      uuid );

   inline nucleus::owned< CFStringRef > CFUUIDCreateString( CFUUIDRef uuid )
     {
      return Nitrogen::CFUUIDCreateString( kCFAllocatorDefault, uuid );
     }

   class CFUUIDGetConstantUUIDWithBytes_Failed {};
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
                                             UInt8          byte15 );

   inline CFUUIDRef CFUUIDGetConstantUUIDWithBytes( UInt8 byte0,
                                                    UInt8 byte1,
                                                    UInt8 byte2,
                                                    UInt8 byte3,
                                                    UInt8 byte4,
                                                    UInt8 byte5,
                                                    UInt8 byte6,
                                                    UInt8 byte7,
                                                    UInt8 byte8,
                                                    UInt8 byte9,
                                                    UInt8 byte10,
                                                    UInt8 byte11,
                                                    UInt8 byte12,
                                                    UInt8 byte13,
                                                    UInt8 byte14,
                                                    UInt8 byte15 )
     {
      return Nitrogen::CFUUIDGetConstantUUIDWithBytes( kCFAllocatorDefault,
                                                       byte0,  byte1,  byte2,  byte3,
                                                       byte4,  byte5,  byte6,  byte7,
                                                       byte8,  byte9,  byte10, byte11,
                                                       byte12, byte13, byte14, byte15 );
     }

   using ::CFUUIDGetUUIDBytes;

   class CFUUIDCreateFromUUIDBytes_Failed {};
   nucleus::owned<CFUUIDRef> CFUUIDCreateFromUUIDBytes( CFAllocatorRef alloc,
                                                        CFUUIDBytes    bytes );
   
   inline nucleus::owned< CFUUIDRef > CFUUIDCreateFromUUIDBytes( CFUUIDBytes bytes )
     {
      return Nitrogen::CFUUIDCreateFromUUIDBytes( kCFAllocatorDefault, bytes );
     }
  }

#endif
