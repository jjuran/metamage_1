// Nitrogen/CFString.cc
// --------------------

// Part of the Nitrogen project.
//
// Written 2002-2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFString.hh"


namespace Nitrogen
  {
   nucleus::owned<CFStringRef> CFStringCreateWithPascalString( CFAllocatorRef     alloc,
                                                      ConstStr255Param   pStr,
                                                      CFStringEncoding   encoding )
     {
      CFStringRef result = ::CFStringCreateWithPascalString( alloc, pStr, encoding );

      if ( result == 0 )
         throw CFStringCreateWithPascalString_Failed();
      
      return nucleus::owned<CFStringRef>::seize( result );
     }

   nucleus::owned<CFStringRef> CFStringCreateWithCString( CFAllocatorRef alloc,
                                                 const char *cStr,
                                                 CFStringEncoding encoding )
     {
      CFStringRef result = ::CFStringCreateWithCString ( alloc, cStr, encoding );

      if ( result == 0 )
         throw CFStringCreateWithCString_Failed ();
      
      return nucleus::owned<CFStringRef>::seize( result );
     }
    
   nucleus::owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                    const UniChar *  chars,
                                                    CFIndex          numChars )
     {
      CFStringRef result = ::CFStringCreateWithCharacters( alloc, chars, numChars );

      if ( result == 0 )
         throw CFStringCreateWithCharacters_Failed();
      
      return nucleus::owned<CFStringRef>::seize( result );
     }

   nucleus::owned< CFStringRef > CFStringCreateWithBytes( CFAllocatorRef     alloc,
                                                 const UInt8 *      bytes,
                                                 CFIndex            numBytes,
                                                 CFStringEncoding   encoding,
                                                 bool               isExternalRepresentation )
     {
      CFStringRef result = ::CFStringCreateWithBytes( alloc,
                                                      bytes,
                                                      numBytes,
                                                      encoding,
                                                      isExternalRepresentation );
      if ( result == 0 )
         throw CFStringCreateWithBytes_Failed();
      
      return nucleus::owned<CFStringRef>::seize( result );
     }
  }