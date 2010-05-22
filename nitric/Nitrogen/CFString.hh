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

// iota
#include "iota/string_traits.hh"

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
   template <> struct disposer_traits< CFMutableStringRef >: disposer_traits< Nitrogen::CFTypeRef > {};
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

   template < class String >
   inline nucleus::owned< CFStringRef > CFStringCreateWithBytes( const String&     string,
                                                                 CFStringEncoding  encoding,
                                                                 bool              isExternalRepresentation )
     {
      return Nitrogen::CFStringCreateWithBytes( reinterpret_cast< const UInt8* >( iota::get_string_data( string ) ),
                                                nucleus::convert< CFIndex      >( iota::get_string_size( string ) ),
                                                encoding,
                                                isExternalRepresentation );
     }

   class CFStringCreateWithFormat_Failed {};
   using ::CFStringGetDoubleValue;
  }

namespace nucleus
  {
   template <>
   struct converter< Nitrogen::UniString, CFStringRef >
     {
      typedef CFStringRef          argument_type;
      typedef Nitrogen::UniString  result_type;
      
      Nitrogen::UniString operator()( const CFStringRef& in ) const
        {
         return Nitrogen::CFStringGetCharacters( in );
        }
     };
   
   template <>
   struct converter< nucleus::owned< CFStringRef >, Nitrogen::UniString >
     {
      typedef Nitrogen::UniString            argument_type;
      typedef nucleus::owned< CFStringRef >  result_type;
      
      nucleus::owned<CFStringRef> operator()( const Nitrogen::UniString& in ) const
        {
         return Nitrogen::CFStringCreateWithCharacters( in );
        }
     };
   
   template <>
   struct converter< nucleus::owned< CFStringRef >, ConstStr255Param >
     {
      typedef ConstStr255Param               argument_type;
      typedef nucleus::owned< CFStringRef >  result_type;
      
      private:
         CFStringEncoding encoding;
      
      public:
         converter( CFStringEncoding e )
           : encoding( e )
           {}
         
         nucleus::owned<CFStringRef> operator()( ConstStr255Param in ) const
           {
            return Nitrogen::CFStringCreateWithPascalString( in, encoding );
           }
     };
   
   template <>
   struct converter< nucleus::owned< CFStringRef >, const char* >
     {
      typedef const char*                    argument_type;
      typedef nucleus::owned< CFStringRef >  result_type;
      
      private:
         CFStringEncoding encoding;
      
      public:
         converter( CFStringEncoding e )
           : encoding( e )
           {}
         
         nucleus::owned<CFStringRef> operator()( const char *in ) const
           {
            return Nitrogen::CFStringCreateWithCString( in, encoding );
           }
     };
   
   template <>
   struct converter< nucleus::owned< CFStringRef >, double >
     {
      typedef double                         argument_type;
      typedef nucleus::owned< CFStringRef >  result_type;
      
      nucleus::owned<CFStringRef> operator()( const double& in ) const
        {
         CFStringRef result = ::CFStringCreateWithFormat( kCFAllocatorDefault, 0, CFSTR("%g"), in );
         if ( result == 0 )
            throw Nitrogen::CFStringCreateWithFormat_Failed();
         return nucleus::owned<CFStringRef>::seize( result );
        }
     };
   
   template <>
   struct converter< double, CFStringRef >
     {
      typedef CFStringRef  argument_type;
      typedef double       result_type;
      
      double operator()( const CFStringRef& in ) const
        {
         return Nitrogen::CFStringGetDoubleValue( in );
        }
     };
  }

#endif
