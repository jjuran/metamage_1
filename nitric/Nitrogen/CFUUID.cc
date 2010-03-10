// Nitrogen/CFUUID.cc
// ------------------

// Part of the Nitrogen project.
//
// Written 2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/CFUUID.hh"


namespace Nitrogen
  {
   nucleus::owned< CFUUIDRef > CFUUIDCreate( CFAllocatorRef alloc )
     {
      CFUUIDRef result = ::CFUUIDCreate( alloc );
      if ( result == 0 )
         throw CFUUIDCreate_Failed();
      return nucleus::owned< CFUUIDRef >::seize( result );
     }

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
                                                      UInt8          byte15 )
     {
      CFUUIDRef result = ::CFUUIDCreateWithBytes( alloc,
                                                  byte0,  byte1,  byte2,  byte3,
                                                  byte4,  byte5,  byte6,  byte7,
                                                  byte8,  byte9,  byte10, byte11,
                                                  byte12, byte13, byte14, byte15 );
      if ( result == 0 )
         throw CFUUIDCreateWithBytes_Failed();
      return nucleus::owned< CFUUIDRef >::seize( result );
     }

   nucleus::owned< CFUUIDRef > CFUUIDCreateFromString( CFAllocatorRef alloc,
                                                       CFStringRef    uuidStr )
     {
      CFUUIDRef result = ::CFUUIDCreateFromString( alloc, uuidStr );
      if ( result == 0 )
         throw CFUUIDCreateFromString_Failed();
      return nucleus::owned< CFUUIDRef >::seize( result );
     }

   nucleus::owned< CFStringRef > CFUUIDCreateString( CFAllocatorRef alloc,
                                                     CFUUIDRef      uuid )
     {
      CFStringRef result = ::CFUUIDCreateString( alloc, uuid );
      if ( result == 0 )
         throw CFUUIDCreateString_Failed();
      return nucleus::owned< CFStringRef >::seize( result );
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

   nucleus::owned<CFUUIDRef> CFUUIDCreateFromUUIDBytes( CFAllocatorRef alloc,
                                                        CFUUIDBytes    bytes )
     {
      CFUUIDRef result = ::CFUUIDCreateFromUUIDBytes( alloc, bytes );
      if ( result == 0 )
         throw CFUUIDCreateFromUUIDBytes_Failed();
      return nucleus::owned< CFUUIDRef >::seize( result );
     }
  }
