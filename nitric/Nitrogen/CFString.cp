// Nitrogen/CFString.cp
// --------------------

// Part of the Nitrogen project.
//
// Written 2002-2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFString.h"


namespace Nitrogen
  {
   Nucleus::Owned<CFStringRef> CFStringCreateWithPascalString( CFAllocatorRef     alloc,
                                                      ConstStr255Param   pStr,
                                                      CFStringEncoding   encoding )
     {
      CFStringRef result = ::CFStringCreateWithPascalString( alloc, pStr, encoding );

      if ( result == 0 )
         throw CFStringCreateWithPascalString_Failed();
      
      return Nucleus::Owned<CFStringRef>::Seize( result );
     }

   Nucleus::Owned<CFStringRef> CFStringCreateWithCString( CFAllocatorRef alloc,
                                                 const char *cStr,
                                                 CFStringEncoding encoding )
     {
      CFStringRef result = ::CFStringCreateWithCString ( alloc, cStr, encoding );

      if ( result == 0 )
         throw CFStringCreateWithCString_Failed ();
      
      return Nucleus::Owned<CFStringRef>::Seize( result );
     }
    
   Nucleus::Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                    const UniChar *  chars,
                                                    CFIndex          numChars )
     {
      CFStringRef result = ::CFStringCreateWithCharacters( alloc, chars, numChars );

      if ( result == 0 )
         throw CFStringCreateWithCharacters_Failed();
      
      return Nucleus::Owned<CFStringRef>::Seize( result );
     }

   inline Nucleus::Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                           const UniString& string )
     {
      return Nitrogen::CFStringCreateWithCharacters( alloc,
                                                     string.data(),
                                                     Nucleus::Convert<CFIndex>( string.size() ) );
     }

   UniString CFStringGetCharacters( CFStringRef theString, CFRange range )
     {
      if ( const UniChar *characters = CFStringGetCharactersPtr(theString) )
         return UniString( characters + range.location,
                           characters + range.location + range.length );
      
      // Since &*result.begin() is undefined for empty strings:
      if ( range.length == 0 )
         return UniString();
      
      UniString result( Nucleus::Convert<UniString::size_type>( range.length ), UniChar() );
      Nitrogen::CFStringGetCharacters( theString,
                                       range,
                                       &*result.begin() );
      return result;
     }

   Nucleus::Owned< CFStringRef > CFStringCreateWithBytes( CFAllocatorRef     alloc,
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
      
      return Nucleus::Owned<CFStringRef>::Seize( result );
     }
   
   UniString CFStringGetCharacters( CFStringRef theString )
     {
      return CFStringGetCharacters( theString, CFRangeMake( 0, Nitrogen::CFStringGetLength( theString ) ) );
     }
  }