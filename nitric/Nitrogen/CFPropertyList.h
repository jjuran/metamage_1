// CFPropertyList.h

#ifndef NITROGEN_CFPROPERTYLIST_H
#define NITROGEN_CFPROPERTYLIST_H

#ifndef __CFPROPERTYLIST__
#include <CFPropertyList.h>
#endif
#ifndef __CFDATE__
#include <CFDate.h>
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CFBOOLEAN_H
#include "Nitrogen/CFBoolean.h"
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

namespace Nitrogen
  {
   template < class CF > struct CFProperty_Traits;
   
   template < class CF >
   struct Basic_CFProperty_Traits
     {
      static ::CFPropertyListRef ConvertToCFPropertyRef( CF value )  { return CFType_Traits<CF>::ConvertToCFTypeRef( value ); }
     };
   
// These declarations should be scattered to the appropriate files:
   using ::CFDateRef;
   using ::CFArrayRef;
   using ::CFDictionaryRef;
   template <> struct CFType_Traits< CFDateRef       >: Basic_CFType_Traits< CFDateRef,       ::CFDateGetTypeID       > {};
   template <> struct CFType_Traits< CFArrayRef      >: Basic_CFType_Traits< CFArrayRef,      ::CFArrayGetTypeID      > {};
   template <> struct CFType_Traits< CFDictionaryRef >: Basic_CFType_Traits< CFDictionaryRef, ::CFDictionaryGetTypeID > {};

// These can stay here:   
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
         CFTypeRef GetCFTypeRef() const															            { return value; }
         
         friend bool operator==( const CFPropertyListRef& a, const CFPropertyListRef& b )       { return a.Get() == b.Get(); }
         friend bool operator!=( const CFPropertyListRef& a, const CFPropertyListRef& b )       { return a.Get() != b.Get(); }
         
         friend bool operator==( const CFPropertyListRef& a, const ::CFPropertyListRef& b )     { return a.Get() == b; }
         friend bool operator!=( const CFPropertyListRef& a, const ::CFPropertyListRef& b )     { return a.Get() != b; }

         friend bool operator==( const ::CFPropertyListRef& a, const CFPropertyListRef& b )     { return a == b.Get(); }
         friend bool operator!=( const ::CFPropertyListRef& a, const CFPropertyListRef& b )     { return a != b.Get(); }
     };

   template < class T > bool operator==( const CFPropertyListRef& a, T const& b )               { return a.GetCFTypeRef() == CFTypeRef( b ); }
   template < class T > bool operator!=( const CFPropertyListRef& a, T const& b )               { return a.GetCFTypeRef() != CFTypeRef( b ); }
 
   template < class T > bool operator==( T const& a, const CFPropertyListRef& b )               { return CFTypeRef( a ) == b.GetCFTypeRef(); }
   template < class T > bool operator!=( T const& a, const CFPropertyListRef& b )               { return CFTypeRef( a ) != b.GetCFTypeRef(); }

   template <> struct OwnedDefaults<CFPropertyListRef> : OwnedDefaults<CFTypeRef>  {};
   

   template <> CFPropertyListRef CFCast<CFPropertyListRef>( CFTypeRef p );

   template <>
   struct Converter< bool, CFPropertyListRef >: public std::unary_function< CFPropertyListRef, bool >
     {
      bool operator()( const ::CFPropertyListRef& in ) const
        {
         return Convert< bool >( CFCast<CFBooleanRef>( in ) );
        }
     };

   template <>
   struct Converter< Owned<CFPropertyListRef>, bool >: public std::unary_function< bool, Owned<CFPropertyListRef> >
     {
      Owned<CFPropertyListRef> operator()( const bool& in ) const
        {
         return Owned<CFPropertyListRef>( Convert< Owned<CFBooleanRef> >( in ) );
         // The explicit conversion to Owned<CFPropertyListRef> is a workaround 
         // for a bug in gcc 3.1 20020420; it rejects the implicit conversion.
        }
     };
  }

#endif
