// CFString.h

#ifndef NITROGEN_CFSTRING_H
#define NITROGEN_CFSTRING_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFSTRING__
#include FRAMEWORK_HEADER(CoreFoundation,CFString.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFARRAY_H
#include "Nitrogen/CFArray.h"
#endif
#ifndef NITROGEN_CFDATA_H
#include "Nitrogen/CFData.h"
#endif
#ifndef NITROGEN_CFDICTIONARY_H
#include "Nitrogen/CFDictionary.h"
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

   inline void CFShow( const CFStringRef s )        { ::CFShow( s ); }

   using ::CFMutableStringRef;
   template <> struct CFType_Traits< CFMutableStringRef >: Basic_CFType_Traits< CFMutableStringRef, ::CFStringGetTypeID > {};
   template <> struct OwnedDefaults< CFMutableStringRef >: OwnedDefaults<CFTypeRef> {};
   inline void CFShow( const CFMutableStringRef s )        { ::CFShow( s ); }

   using ::CFStringGetTypeID;
   
   class CFStringCreateWithPascalString_Failed {};
   Owned<CFStringRef> CFStringCreateWithPascalString( CFAllocatorRef     alloc,
                                                      ConstStr255Param   pStr,
                                                      CFStringEncoding   encoding );
   
   inline Owned<CFStringRef> CFStringCreateWithPascalString( ConstStr255Param   pStr,
                                                             CFStringEncoding   encoding )
     {
      return Nitrogen::CFStringCreateWithPascalString( kCFAllocatorDefault, pStr, encoding );
     }

   class CFStringCreateWithCString_Failed {};
   Owned<CFStringRef> CFStringCreateWithCString( CFAllocatorRef alloc,
                                                 const char *cStr,
                                                 CFStringEncoding encoding );

   inline Owned<CFStringRef> CFStringCreateWithCString( const char *cStr,
                                                        CFStringEncoding encoding )
     {
      return Nitrogen::CFStringCreateWithCString( kCFAllocatorDefault, cStr, encoding );
     }
   
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
   
   class CFStringCreateWithBytes_Failed {};
   
   Owned< CFStringRef > CFStringCreateWithBytes( CFAllocatorRef     alloc,
                                                 const UInt8 *      bytes,
                                                 CFIndex            numBytes,
                                                 CFStringEncoding   encoding,
                                                 bool               isExternalRepresentation );

   inline Owned< CFStringRef > CFStringCreateWithBytes( const UInt8 *      bytes,
                                                        CFIndex            numBytes,
                                                        CFStringEncoding   encoding,
                                                        bool               isExternalRepresentation )
     {
      return Nitrogen::CFStringCreateWithBytes( kCFAllocatorDefault,
                                                bytes,
                                                numBytes,
                                                encoding,
                                                isExternalRepresentation );
     }

   inline Owned< CFStringRef > CFStringCreateWithBytes( const std::string& string,
                                                        CFStringEncoding   encoding,
                                                        bool               isExternalRepresentation )
     {
      return Nitrogen::CFStringCreateWithBytes( reinterpret_cast< const UInt8 * >( string.data() ),
                                                Convert<CFIndex>( string.size() ),
                                                encoding,
                                                isExternalRepresentation );
     }

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
   struct Converter< Owned<CFStringRef>, ConstStr255Param >: public std::unary_function< ConstStr255Param, Owned<CFStringRef> >
     {
      private:
         CFStringEncoding encoding;
      
      public:
         Converter( CFStringEncoding e )
           : encoding( e )
           {}
         
         Owned<CFStringRef> operator()( ConstStr255Param in ) const
           {
            return CFStringCreateWithPascalString( in, encoding );
           }
     };
   
   template <>
   struct Converter< Owned<CFStringRef>, const char * >: public std::unary_function< const char *, Owned<CFStringRef> >
     {
      private:
         CFStringEncoding encoding;
      
      public:
         Converter( CFStringEncoding e )
           : encoding( e )
           {}
         
         Owned<CFStringRef> operator()( const char *in ) const
           {
            return CFStringCreateWithCString( in, encoding );
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
