// CFString.h

#ifndef NITROGEN_CFSTRING_H
#define NITROGEN_CFSTRING_H

#ifndef __CFSTRING__
#include <CFString.h>
#endif
#ifndef __CFARRAY__
#include <CFArray.h>
#endif
#ifndef __CFDATA__
#include <CFData.h>
#endif
#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif

#include <string>

namespace Nitrogen
  {
   /* These declararions are in Nitrogen/CFBase.h:
      using ::CFStringRef;
      template <> struct CFType_Traits< CFStringRef >: Basic_CFType_Traits< CFStringRef, ::CFStringGetTypeID > {};
      template <> struct OwnedDefaults< CFStringRef >: OwnedDefaults<CFTypeRef> {};
   */
   
   class CFStringCreateWithCharacters_Failed {};
   Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                    const UniChar *  chars,
                                                    CFIndex          numChars );

   inline Owned<CFStringRef> CFStringCreateWithCharacters( const UniChar *  chars,
                                                           CFIndex          numChars )
     {
      return Nitrogen::CFStringCreateWithCharacters( kCFAllocatorDefault, chars, numChars );
     }

   Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                    const UniString& string );

   inline Owned<CFStringRef> CFStringCreateWithCharacters( const UniString& string )
     {
      return Nitrogen::CFStringCreateWithCharacters( kCFAllocatorDefault, string );
     }

   inline CFIndex CFStringGetLength( CFStringRef theString )
     {
      return ::CFStringGetLength( theString );
     }

   inline void CFStringGetCharacters( CFStringRef   theString,
                                      CFRange       range,
                                      UniChar *     buffer )
     {
      ::CFStringGetCharacters( theString, range, buffer );
     }
   
   UniString CFStringGetCharacters( CFStringRef theString, CFRange range );
   UniString CFStringGetCharacters( CFStringRef theString );
   
   class CFStringCreateWithFormat_Failed {};
   
   template <>
   struct Converter< UniString, CFStringRef >: public std::unary_function< CFStringRef, UniString >
     {
      UniString operator()( const CFStringRef& in ) const
        {
         return CFStringGetCharacters( in );
        }
     };
   
   template <>
   struct Converter< Owned<CFStringRef>, UniString >: public std::unary_function< UniString, Owned<CFStringRef> >
     {
      Owned<CFStringRef> operator()( const UniString& in ) const
        {
         return CFStringCreateWithCharacters( in );
        }
     };
   
   template <>
   struct Converter< Owned<CFStringRef>, double >: public std::unary_function< double, Owned<CFStringRef> >
     {
      Owned<CFStringRef> operator()( const double& in ) const
        {
         CFStringRef result = ::CFStringCreateWithFormat( kCFAllocatorDefault, 0, CFSTR("%g"), in );
         if ( result == 0 )
            throw CFStringCreateWithFormat_Failed();
         return Owned<CFStringRef>::Seize( result );
        }
     };
   
   template <>
   struct Converter< double, CFStringRef >: public std::unary_function< CFStringRef, double >
     {
      double operator()( const CFStringRef& in ) const
        {
         return CFStringGetDoubleValue( in );
        }
     };
  }

#endif
