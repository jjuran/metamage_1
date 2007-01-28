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
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
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
   inline void CFShow( const CFMutableStringRef s )        { ::CFShow( s ); }

	using ::CFStringEncoding;
   }

namespace Nucleus {
   template <> struct OwnedDefaults< Nitrogen::CFMutableStringRef >: OwnedDefaults< Nitrogen::CFTypeRef > {};
   }
   
namespace Nitrogen {
   using ::CFStringGetTypeID;
   
   class CFStringCreateWithPascalString_Failed {};
   Nucleus::Owned<CFStringRef> CFStringCreateWithPascalString( CFAllocatorRef     alloc,
                                                               ConstStr255Param   pStr,
                                                               CFStringEncoding   encoding );
   
   inline Nucleus::Owned<CFStringRef> CFStringCreateWithPascalString( ConstStr255Param   pStr,
                                                                      CFStringEncoding   encoding )
     {
      return Nitrogen::CFStringCreateWithPascalString( kCFAllocatorDefault, pStr, encoding );
     }

   class CFStringCreateWithCString_Failed {};
   Nucleus::Owned<CFStringRef> CFStringCreateWithCString( CFAllocatorRef alloc,
                                                          const char *cStr,
                                                          CFStringEncoding encoding );

   inline Nucleus::Owned<CFStringRef> CFStringCreateWithCString( const char *cStr,
                                                                 CFStringEncoding encoding )
     {
      return Nitrogen::CFStringCreateWithCString( kCFAllocatorDefault, cStr, encoding );
     }
   
   class CFStringCreateWithCharacters_Failed {};
   Nucleus::Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                             const UniChar *  chars,
                                                             CFIndex          numChars );

   inline Nucleus::Owned<CFStringRef> CFStringCreateWithCharacters( const UniChar *  chars,
                                                                    CFIndex          numChars )
     {
      return Nitrogen::CFStringCreateWithCharacters( kCFAllocatorDefault, chars, numChars );
     }

   Nucleus::Owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                             const UniString& string );

   inline Nucleus::Owned<CFStringRef> CFStringCreateWithCharacters( const UniString& string )
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
   
   Nucleus::Owned< CFStringRef > CFStringCreateWithBytes( CFAllocatorRef     alloc,
                                                          const UInt8 *      bytes,
                                                          CFIndex            numBytes,
                                                          CFStringEncoding   encoding,
                                                          bool               isExternalRepresentation );

   inline Nucleus::Owned< CFStringRef > CFStringCreateWithBytes( const UInt8 *      bytes,
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

   inline Nucleus::Owned< CFStringRef > CFStringCreateWithBytes( const std::string& string,
                                                                 CFStringEncoding   encoding,
                                                                 bool               isExternalRepresentation )
     {
      return Nitrogen::CFStringCreateWithBytes( reinterpret_cast< const UInt8 * >( string.data() ),
                                                Nucleus::Convert<CFIndex>( string.size() ),
                                                encoding,
                                                isExternalRepresentation );
     }

   class CFStringCreateWithFormat_Failed {};
   using ::CFStringGetDoubleValue;
  }

namespace Nucleus
  {
   template <>
   struct Converter< Nitrogen::UniString, Nitrogen::CFStringRef >: public std::unary_function< Nitrogen::CFStringRef, Nitrogen::UniString >
     {
      Nitrogen::UniString operator()( const Nitrogen::CFStringRef& in ) const
        {
         return Nitrogen::CFStringGetCharacters( in );
        }
     };
   
   template <>
   struct Converter< Nucleus::Owned<Nitrogen::CFStringRef>, Nitrogen::UniString >: public std::unary_function< Nitrogen::UniString, Nucleus::Owned<Nitrogen::CFStringRef> >
     {
      Nucleus::Owned<Nitrogen::CFStringRef> operator()( const Nitrogen::UniString& in ) const
        {
         return Nitrogen::CFStringCreateWithCharacters( in );
        }
     };
   
   template <>
   struct Converter< Nucleus::Owned<Nitrogen::CFStringRef>, ConstStr255Param >: public std::unary_function< ConstStr255Param, Nucleus::Owned<Nitrogen::CFStringRef> >
     {
      private:
         Nitrogen::CFStringEncoding encoding;
      
      public:
         Converter( Nitrogen::CFStringEncoding e )
           : encoding( e )
           {}
         
         Nucleus::Owned<Nitrogen::CFStringRef> operator()( ConstStr255Param in ) const
           {
            return Nitrogen::CFStringCreateWithPascalString( in, encoding );
           }
     };
   
   template <>
   struct Converter< Nucleus::Owned<Nitrogen::CFStringRef>, const char * >: public std::unary_function< const char *, Nucleus::Owned<Nitrogen::CFStringRef> >
     {
      private:
         Nitrogen::CFStringEncoding encoding;
      
      public:
         Converter( Nitrogen::CFStringEncoding e )
           : encoding( e )
           {}
         
         Nucleus::Owned<Nitrogen::CFStringRef> operator()( const char *in ) const
           {
            return Nitrogen::CFStringCreateWithCString( in, encoding );
           }
     };
   
   template <>
   struct Converter< Nucleus::Owned<Nitrogen::CFStringRef>, double >: public std::unary_function< double, Nucleus::Owned<Nitrogen::CFStringRef> >
     {
      Nucleus::Owned<Nitrogen::CFStringRef> operator()( const double& in ) const
        {
         CFStringRef result = ::CFStringCreateWithFormat( kCFAllocatorDefault, 0, CFSTR("%g"), in );
         if ( result == 0 )
            throw Nitrogen::CFStringCreateWithFormat_Failed();
         return Nucleus::Owned<Nitrogen::CFStringRef>::Seize( result );
        }
     };
   
   template <>
   struct Converter< double, Nitrogen::CFStringRef >: public std::unary_function< Nitrogen::CFStringRef, double >
     {
      double operator()( const Nitrogen::CFStringRef& in ) const
        {
         return Nitrogen::CFStringGetDoubleValue( in );
        }
     };
  }

#endif
