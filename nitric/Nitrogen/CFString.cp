// CFString.cp

#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif

namespace Nitrogen
  {
   Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                    const UniChar *  chars,
                                                    CFIndex          numChars )
     {
      CFStringRef result = ::CFStringCreateWithCharacters( alloc, chars, numChars );

      if ( result == 0 )
         throw CFStringCreateWithCharacters_Failed();
      
      return Owned<CFStringRef>::Seize( result );
     }

   inline Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                           const UniString& string )
     {
      return Nitrogen::CFStringCreateWithCharacters( alloc,
                                                     string.data(),
                                                     Convert<CFIndex>( string.size() ) );
     }

   UniString CFStringGetCharacters( CFStringRef theString, CFRange range )
     {
      if ( const UniChar *characters = CFStringGetCharactersPtr(theString) )
         return UniString( characters + range.location,
                           characters + range.location + range.length );
      
      // Since &*result.begin() is undefined for empty strings:
      if ( range.length == 0 )
         return UniString();
      
      UniString result( Convert<UniString::size_type>( range.length ), UniChar() );
      Nitrogen::CFStringGetCharacters( theString,
                                       range,
                                       &*result.begin() );
      return result;
     }
   
   UniString CFStringGetCharacters( CFStringRef theString )
     {
      return CFStringGetCharacters( theString, CFRangeMake( 0, Nitrogen::CFStringGetLength( theString ) ) );
     }
  }