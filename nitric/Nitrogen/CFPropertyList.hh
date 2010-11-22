// Nitrogen/CFPropertyList.hh
// --------------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFPROPERTYLIST_HH
#define NITROGEN_CFPROPERTYLIST_HH

#ifndef __CFPROPERTYLIST__
#include <CFPropertyList.h>
#endif

#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif
#ifndef NITROGEN_CFBOOLEAN_HH
#include "Nitrogen/CFBoolean.hh"
#endif
#ifndef NITROGEN_CFDATE_HH
#include "Nitrogen/CFDate.hh"
#endif
#ifndef NITROGEN_CFSTRING_HH
#include "Nitrogen/CFString.hh"
#endif
#ifndef NITROGEN_CFNUMBER_HH
#include "Nitrogen/CFNumber.hh"
#endif
#ifndef NITROGEN_CFDATA_HH
#include "Nitrogen/CFData.hh"
#endif
#ifndef NITROGEN_CFDICTIONARY_HH
#include "Nitrogen/CFDictionary.hh"
#endif
#ifndef NITROGEN_CFARRAY_HH
#include "Nitrogen/CFArray.hh"
#endif

namespace Nitrogen
  {
   template < class CF > struct CFProperty_Traits;
   
   template < class CF >
   struct Basic_CFProperty_Traits
     {
      static ::CFPropertyListRef ConvertToCFPropertyRef( CF value )  { return CFType_Traits<CF>::ConvertToCFTypeRef( value ); }
     };

   template <> struct CFProperty_Traits< CFBooleanRef    >: Basic_CFProperty_Traits< CFBooleanRef    > {};
   template <> struct CFProperty_Traits< CFStringRef     >: Basic_CFProperty_Traits< CFStringRef     > {};
   template <> struct CFProperty_Traits< CFNumberRef     >: Basic_CFProperty_Traits< CFNumberRef     > {};
   template <> struct CFProperty_Traits< CFDateRef       >: Basic_CFProperty_Traits< CFDateRef       > {};
   template <> struct CFProperty_Traits< CFDataRef       >: Basic_CFProperty_Traits< CFDataRef       > {};
   template <> struct CFProperty_Traits< CFArrayRef      >: Basic_CFProperty_Traits< CFArrayRef      > {};
   template <> struct CFProperty_Traits< CFDictionaryRef >: Basic_CFProperty_Traits< CFDictionaryRef > {};

   class CFPropertyListRef
     {
      private:
         ::CFPropertyListRef value;
      
      public:
         CFPropertyListRef()                             : value( 0 )       {}
         CFPropertyListRef( ::CFPropertyListRef v )      : value( v )       {}

         template < class T >
         CFPropertyListRef( T* v )                       : value( CFProperty_Traits< T* >::ConvertToCFPropertyRef( v ) )    {}
         
         static CFPropertyListRef Make( ::CFPropertyListRef v )                                 { return CFPropertyListRef( v ); }
         ::CFPropertyListRef Get() const                                                        { return value; }
         operator ::CFPropertyListRef() const                                                   { return value; }
      
         operator CFTypeRef() const                                                             { return value; }
         CFTypeRef GetCFTypeRef() const                                                         { return value; }
         
         friend bool operator==( const CFPropertyListRef& a, const CFPropertyListRef& b )       { return a.Get() == b.Get(); }
         friend bool operator!=( const CFPropertyListRef& a, const CFPropertyListRef& b )       { return a.Get() != b.Get(); }
         
         friend bool operator==( const CFPropertyListRef& a, const ::CFPropertyListRef& b )     { return a.Get() == b; }
         friend bool operator!=( const CFPropertyListRef& a, const ::CFPropertyListRef& b )     { return a.Get() != b; }

         friend bool operator==( const ::CFPropertyListRef& a, const CFPropertyListRef& b )     { return a == b.Get(); }
         friend bool operator!=( const ::CFPropertyListRef& a, const CFPropertyListRef& b )     { return a != b.Get(); }
     };

  }

namespace nucleus
  {
   template <> struct disposer_class<Nitrogen::CFPropertyListRef> : disposer_class<Nitrogen::CFTypeRef>  {};
  }

namespace Nitrogen
  {
   template <> CFPropertyListRef CFCast<CFPropertyListRef>( CFTypeRef p );
  }

namespace nucleus
  {
   template <>
   struct converter< bool, Nitrogen::CFPropertyListRef >
     {
      typedef Nitrogen::CFPropertyListRef  argument_type;
      typedef bool                         result_type;
      
      bool operator()( const ::CFPropertyListRef& in ) const
        {
         return convert< bool >( Nitrogen::CFCast<CFBooleanRef>( in ) );
        }
     };

   template <>
   struct converter< nucleus::owned<Nitrogen::CFPropertyListRef>, bool >
     {
      typedef bool                                           argument_type;
      typedef nucleus::owned< Nitrogen::CFPropertyListRef >  result_type;
      
      nucleus::owned<Nitrogen::CFPropertyListRef> operator()( const bool& in ) const
        {
         return nucleus::owned<Nitrogen::CFPropertyListRef>( convert< nucleus::owned<CFBooleanRef> >( in ) );
         // The explicit conversion to nucleus::owned<CFPropertyListRef> is a workaround 
         // for a bug in gcc 3.1 20020420; it rejects the implicit conversion.
        }
     };
  }

#endif
