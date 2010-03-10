// Nitrogen/CFString.hh
// --------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFSTRING_HH
#define NITROGEN_CFSTRING_HH

#ifndef __CFSTRING__
#include <CFString.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif
#ifndef NITROGEN_CFARRAY_HH
#include "Nitrogen/CFArray.hh"
#endif
#ifndef NITROGEN_CFDATA_HH
#include "Nitrogen/CFData.hh"
#endif
#ifndef NITROGEN_CFDICTIONARY_HH
#include "Nitrogen/CFDictionary.hh"
#endif
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif

#include <string>

namespace Nitrogen
  {
   /* These declararions are in Nitrogen/CFBase.hh:
      using ::CFStringRef;
      template <> struct CFType_Traits< CFStringRef >: Basic_CFType_Traits< CFStringRef, ::CFStringGetTypeID > {};
      template <> struct disposer_traits< CFStringRef >: disposer_traits<CFTypeRef> {};
   */

   inline void CFShow( const CFStringRef s )        { ::CFShow( s ); }

   using ::CFMutableStringRef;
   template <> struct CFType_Traits< CFMutableStringRef >: Basic_CFType_Traits< CFMutableStringRef, ::CFStringGetTypeID > {};
   inline void CFShow( const CFMutableStringRef s )        { ::CFShow( s ); }

	using ::CFStringEncoding;
   }

namespace nucleus {
   template <> struct disposer_traits< Nitrogen::CFMutableStringRef >: disposer_traits< Nitrogen::CFTypeRef > {};
   }
   
namespace Nitrogen {
   using ::CFStringGetTypeID;
   
   class CFStringCreateWithPascalString_Failed {};
   nucleus::owned<CFStringRef> CFStringCreateWithPascalString( CFAllocatorRef     alloc,
                                                               ConstStr255Param   pStr,
                                                               CFStringEncoding   encoding );
   
   inline nucleus::owned<CFStringRef> CFStringCreateWithPascalString( ConstStr255Param   pStr,
                                                                      CFStringEncoding   encoding )
     {
      return Nitrogen::CFStringCreateWithPascalString( kCFAllocatorDefault, pStr, encoding );
     }

   class CFStringCreateWithCString_Failed {};
   nucleus::owned<CFStringRef> CFStringCreateWithCString( CFAllocatorRef alloc,
                                                          const char *cStr,
                                                          CFStringEncoding encoding );

   inline nucleus::owned<CFStringRef> CFStringCreateWithCString( const char *cStr,
                                                                 CFStringEncoding encoding )
     {
      return Nitrogen::CFStringCreateWithCString( kCFAllocatorDefault, cStr, encoding );
     }
   
   class CFStringCreateWithCharacters_Failed {};
   nucleus::owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                             const UniChar *  chars,
                                                             CFIndex          numChars );

   inline nucleus::owned<CFStringRef> CFStringCreateWithCharacters( const UniChar *  chars,
                                                                    CFIndex          numChars )
     {
      return Nitrogen::CFStringCreateWithCharacters( kCFAllocatorDefault, chars, numChars );
     }

   nucleus::owned<CFStringRef> CFStringCreateWithCharacters( CFAllocatorRef   alloc,
                                                             const UniString& string );

   inline nucleus::owned<CFStringRef> CFStringCreateWithCharacters( const UniString& string )
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
   
   nucleus::owned< CFStringRef > CFStringCreateWithBytes( CFAllocatorRef     alloc,
                                                          const UInt8 *      bytes,
                                                          CFIndex            numBytes,
                                                          CFStringEncoding   encoding,
                                                          bool               isExternalRepresentation );

   inline nucleus::owned< CFStringRef > CFStringCreateWithBytes( const UInt8 *      bytes,
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

   inline nucleus::owned< CFStringRef > CFStringCreateWithBytes( const std::string& string,
                                                                 CFStringEncoding   encoding,
                                                                 bool               isExternalRepresentation )
     {
      return Nitrogen::CFStringCreateWithBytes( reinterpret_cast< const UInt8 * >( string.data() ),
                                                nucleus::convert<CFIndex>( string.size() ),
                                                encoding,
                                                isExternalRepresentation );
     }

   class CFStringCreateWithFormat_Failed {};
   using ::CFStringGetDoubleValue;
  }

namespace nucleus
  {
   template <>
   struct converter< Nitrogen::UniString, Nitrogen::CFStringRef >: public std::unary_function< Nitrogen::CFStringRef, Nitrogen::UniString >
     {
      Nitrogen::UniString operator()( const Nitrogen::CFStringRef& in ) const
        {
         return Nitrogen::CFStringGetCharacters( in );
        }
     };
   
   template <>
   struct converter< nucleus::owned<Nitrogen::CFStringRef>, Nitrogen::UniString >: public std::unary_function< Nitrogen::UniString, nucleus::owned<Nitrogen::CFStringRef> >
     {
      nucleus::owned<Nitrogen::CFStringRef> operator()( const Nitrogen::UniString& in ) const
        {
         return Nitrogen::CFStringCreateWithCharacters( in );
        }
     };
   
   template <>
   struct converter< nucleus::owned<Nitrogen::CFStringRef>, ConstStr255Param >: public std::unary_function< ConstStr255Param, nucleus::owned<Nitrogen::CFStringRef> >
     {
      private:
         Nitrogen::CFStringEncoding encoding;
      
      public:
         converter( Nitrogen::CFStringEncoding e )
           : encoding( e )
           {}
         
         nucleus::owned<Nitrogen::CFStringRef> operator()( ConstStr255Param in ) const
           {
            return Nitrogen::CFStringCreateWithPascalString( in, encoding );
           }
     };
   
   template <>
   struct converter< nucleus::owned<Nitrogen::CFStringRef>, const char * >: public std::unary_function< const char *, nucleus::owned<Nitrogen::CFStringRef> >
     {
      private:
         Nitrogen::CFStringEncoding encoding;
      
      public:
         converter( Nitrogen::CFStringEncoding e )
           : encoding( e )
           {}
         
         nucleus::owned<Nitrogen::CFStringRef> operator()( const char *in ) const
           {
            return Nitrogen::CFStringCreateWithCString( in, encoding );
           }
     };
   
   template <>
   struct converter< nucleus::owned<Nitrogen::CFStringRef>, double >: public std::unary_function< double, nucleus::owned<Nitrogen::CFStringRef> >
     {
      nucleus::owned<Nitrogen::CFStringRef> operator()( const double& in ) const
        {
         CFStringRef result = ::CFStringCreateWithFormat( kCFAllocatorDefault, 0, CFSTR("%g"), in );
         if ( result == 0 )
            throw Nitrogen::CFStringCreateWithFormat_Failed();
         return nucleus::owned<Nitrogen::CFStringRef>::seize( result );
        }
     };
   
   template <>
   struct converter< double, Nitrogen::CFStringRef >: public std::unary_function< Nitrogen::CFStringRef, double >
     {
      double operator()( const Nitrogen::CFStringRef& in ) const
        {
         return Nitrogen::CFStringGetDoubleValue( in );
        }
     };
  }

#endif
