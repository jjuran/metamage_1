// Nitrogen/CFUUID.h
// -----------------

// Part of the Nitrogen project.
//
// Written 2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFUUID_H
#define NITROGEN_CFUUID_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFUUID__
#include FRAMEWORK_HEADER(CoreFoundation,CFUUID.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif

namespace Nitrogen
  {
   using ::CFUUIDRef;
  }

namespace Nucleus
  {
   template <> struct OwnedDefaults< Nitrogen::CFUUIDRef >: OwnedDefaults< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {
   template <> struct CFType_Traits< CFUUIDRef >: Basic_CFType_Traits< CFUUIDRef, ::CFUUIDGetTypeID > {};

   using ::CFUUIDBytes;
   
   using ::CFUUIDGetTypeID;
   
   class CFUUIDCreate_Failed {};
   Nucleus::Owned< CFUUIDRef > CFUUIDCreate( CFAllocatorRef alloc = kCFAllocatorDefault );

   class CFUUIDCreateWithBytes_Failed {};
   Nucleus::Owned< CFUUIDRef > CFUUIDCreateWithBytes( CFAllocatorRef alloc,
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
   
   inline Nucleus::Owned< CFUUIDRef > CFUUIDCreateWithBytes( UInt8 byte0,
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
   Nucleus::Owned< CFUUIDRef > CFUUIDCreateFromString( CFAllocatorRef alloc,
                                              CFStringRef    uuidStr );
   
   inline Nucleus::Owned< CFUUIDRef > CFUUIDCreateFromString( CFStringRef uuidStr )
     {
      return Nitrogen::CFUUIDCreateFromString( kCFAllocatorDefault, uuidStr );
     }

   class CFUUIDCreateString_Failed {};
   Nucleus::Owned< CFStringRef > CFUUIDCreateString( CFAllocatorRef alloc,
                                            CFUUIDRef      uuid );

   inline Nucleus::Owned< CFStringRef > CFUUIDCreateString( CFUUIDRef uuid )
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
   Nucleus::Owned<CFUUIDRef> CFUUIDCreateFromUUIDBytes( CFAllocatorRef alloc,
                                               CFUUIDBytes    bytes );
   
   inline Nucleus::Owned< CFUUIDRef > CFUUIDCreateFromUUIDBytes( CFUUIDBytes bytes )
     {
      return Nitrogen::CFUUIDCreateFromUUIDBytes( kCFAllocatorDefault, bytes );
     }
  }

#endif
