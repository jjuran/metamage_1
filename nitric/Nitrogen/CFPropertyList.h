// Nitrogen/CFPropertyList.h
// -------------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFPROPERTYLIST_H
#define NITROGEN_CFPROPERTYLIST_H

#ifndef __CFPROPERTYLIST__
#include <CFPropertyList.h>
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFBOOLEAN_H
#include "Nitrogen/CFBoolean.h"
#endif
#ifndef NITROGEN_CFDATE_H
#include "Nitrogen/CFDate.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_CFNUMBER_H
#include "Nitrogen/CFNumber.h"
#endif
#ifndef NITROGEN_CFDATA_H
#include "Nitrogen/CFData.h"
#endif
#ifndef NITROGEN_CFDICTIONARY_H
#include "Nitrogen/CFDictionary.h"
#endif
#ifndef NITROGEN_CFARRAY_H
#include "Nitrogen/CFArray.h"
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

namespace Nucleus
  {
   template <> struct Disposer_Traits<Nitrogen::CFPropertyListRef> : Disposer_Traits<Nitrogen::CFTypeRef>  {};
  }

namespace Nitrogen
  {
   template <> CFPropertyListRef CFCast<CFPropertyListRef>( CFTypeRef p );
  }

namespace Nucleus
  {
   template <>
   struct Converter< bool, Nitrogen::CFPropertyListRef >: public std::unary_function< Nitrogen::CFPropertyListRef, bool >
     {
      bool operator()( const ::CFPropertyListRef& in ) const
        {
         return Convert< bool >( Nitrogen::CFCast<Nitrogen::CFBooleanRef>( in ) );
        }
     };

   template <>
   struct Converter< Nucleus::Owned<Nitrogen::CFPropertyListRef>, bool >: public std::unary_function< bool, Nucleus::Owned<Nitrogen::CFPropertyListRef> >
     {
      Nucleus::Owned<Nitrogen::CFPropertyListRef> operator()( const bool& in ) const
        {
         return Nucleus::Owned<Nitrogen::CFPropertyListRef>( Convert< Nucleus::Owned<Nitrogen::CFBooleanRef> >( in ) );
         // The explicit conversion to Nucleus::Owned<CFPropertyListRef> is a workaround 
         // for a bug in gcc 3.1 20020420; it rejects the implicit conversion.
        }
     };
  }

#endif
