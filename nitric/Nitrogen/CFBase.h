// CFBase.h

#ifndef NITROGEN_CFBASE_H
#define NITROGEN_CFBASE_H

#ifndef __CFBASE__
#include <CFBase.h>
#endif
#ifndef __CFSTRING__
#include <CFString.h>
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen
  {
   class CFTypeIDTag {};
   typedef IDType< CFTypeIDTag, ::CFTypeID > CFTypeID;

   template < class T > struct CFType_Traits;
   
   template < class CF, ::CFTypeID (*getTypeID)() >
   struct Basic_CFType_Traits
     {
      static CFTypeID ID()                                    { return getTypeID(); }
      static ::CFTypeRef ConvertToCFTypeRef( CF value )       { return value; }
     };
   
// These declarations should be scattered to the appropriate files:
//   template <> struct CFType_Traits< ::CFBagRef            >: Basic_CFType_Traits< ::CFBagRef,            ::CFBagGetTypeID            > {};
//   template <> struct CFType_Traits< ::CFSetRef            >: Basic_CFType_Traits< ::CFSetRef,            ::CFSetGetTypeID            > {};
//   template <> struct CFType_Traits< ::CFTreeRef           >: Basic_CFType_Traits< ::CFTreeRef,           ::CFTreeGetTypeID           > {};
//   template <> struct CFType_Traits< ::CFCharacterSetRef   >: Basic_CFType_Traits< ::CFCharacterSetRef,   ::CFCharacterSetGetTypeID   > {};
//   template <> struct CFType_Traits< ::CFTimeZoneRef       >: Basic_CFType_Traits< ::CFTimeZoneRef,       ::CFTimeZoneGetTypeID       > {};
//   template <> struct CFType_Traits< ::CFPlugInInstanceRef >: Basic_CFType_Traits< ::CFPlugInInstanceRef, ::CFPlugInInstanceGetTypeID > {};
//   template <> struct CFType_Traits< ::CFURLRef            >: Basic_CFType_Traits< ::CFURLRef,            ::CFURLGetTypeID            > {};
//   template <> struct CFType_Traits< ::CFXMLNodeRef        >: Basic_CFType_Traits< ::CFXMLNodeRef,        ::CFXMLNodeGetTypeID        > {};
//   template <> struct CFType_Traits< ::CFXMLTreeRef        >: Basic_CFType_Traits< ::CFXMLTreeRef,        ::CFXMLTreeGetTypeID        > {};
//   template <> struct CFType_Traits< ::CFXMLParserRef      >: Basic_CFType_Traits< ::CFXMLParserRef,      ::CFXMLParserGetTypeID      > {};

   class CFTypeRef
     {
      private:
         ::CFTypeRef value;
      
      public:
         CFTypeRef()                               : value( 0 ) {}
         CFTypeRef( ::CFTypeRef v )                : value( v ) {}

         template < class T >
         CFTypeRef( T* v )                         : value( CFType_Traits< T* >::ConvertToCFTypeRef( v ) )    {}
         
         static CFTypeRef Make( ::CFTypeRef v )                                    { return CFTypeRef( v ); }
         ::CFTypeRef Get() const                                                   { return value; }
         operator ::CFTypeRef() const                                              { return value; }
      
         friend bool operator==( const CFTypeRef& a, const CFTypeRef& b )          { return a.Get() == b.Get(); }
         friend bool operator!=( const CFTypeRef& a, const CFTypeRef& b )          { return a.Get() != b.Get(); }
         
         friend bool operator==( const CFTypeRef& a, const ::CFTypeRef& b )        { return a.Get() == b; }
         friend bool operator!=( const CFTypeRef& a, const ::CFTypeRef& b )        { return a.Get() != b; }

         friend bool operator==( const ::CFTypeRef& a, const CFTypeRef& b )        { return a == b.Get(); }
         friend bool operator!=( const ::CFTypeRef& a, const CFTypeRef& b )        { return a != b.Get(); }
     };

   template <> struct Disposer<CFTypeRef>: public std::unary_function< CFTypeRef, void >
     {
      void operator()( CFTypeRef cf ) const
        {
         ::CFRelease( cf );
        }
     };
   
   template < class T > bool operator==( const CFTypeRef& a, T const *const& b )   { return a == CFTypeRef( b ); }
   template < class T > bool operator!=( const CFTypeRef& a, T const *const& b )   { return a != CFTypeRef( b ); }
   
   template < class T > bool operator==( T const *const& a, const CFTypeRef& b )   { return CFTypeRef( a ) == b; }
   template < class T > bool operator!=( T const *const& a, const CFTypeRef& b )   { return CFTypeRef( a ) != b; }

   class CFOptionFlagsTag {};
   typedef FlagType< CFOptionFlagsTag, ::CFOptionFlags > CFOptionFlags;
   
   class CFHashCodeTag {};
   typedef IDType< CFHashCodeTag, ::CFHashCode > CFHashCode;
   
   using ::CFStringRef;
   using ::CFAllocatorRef;
   
   template <> struct CFType_Traits< CFStringRef >: Basic_CFType_Traits< CFStringRef, ::CFStringGetTypeID > {};
   template <> struct OwnedDefaults< CFStringRef >: OwnedDefaults<CFTypeRef> {};

   template <> struct CFType_Traits< CFAllocatorRef >: Basic_CFType_Traits< CFAllocatorRef, ::CFAllocatorGetTypeID > {};
   template <> struct OwnedDefaults< CFAllocatorRef >: OwnedDefaults<CFTypeRef> {};


   inline CFTypeID CFGetTypeID( CFTypeRef cf )
     {
      return ::CFGetTypeID( cf );
     }

   inline Owned<CFStringRef> CFCopyTypeIDDescription( CFTypeID cf )
     {
      return Owned<CFStringRef>::Seize( ::CFCopyTypeIDDescription( cf ) );
     }
   
   template < class CF >
   Owned< CF > CFRetain( CF toRetain )
     {
      ::CFRetain( CFTypeRef( toRetain ) );
      return Owned<CF>::Seize( toRetain );
     }

   inline void CFRelease( Owned<CFTypeRef> /*toRelease*/ )
     {
     }
   
   inline CFIndex CFGetRetainCount( CFTypeRef cf )
     {
      return ::CFGetRetainCount( cf );
     }

   inline bool CFEqual( CFTypeRef cf1, CFTypeRef cf2 )
     {
      return ::CFEqual( cf1, cf2 );
     }

   inline CFHashCode CFHash( CFTypeRef cf )
     {
      return ::CFHash( cf );
     }

   inline Owned<CFStringRef> CFCopyDescription( CFTypeRef cf )
     {
      return Owned<CFStringRef>::Seize( ::CFCopyDescription( cf ) );
     }

   inline CFAllocatorRef CFGetAllocator( CFTypeRef cf )
     {
      return ::CFGetAllocator( cf );
     }


   class CFCast_Failed   {};
   
   template < class Desired > Desired CFCast( CFTypeRef p )
     {
      if ( p == 0 )
         return 0;
      
      if ( CFGetTypeID( p ) != CFType_Traits<Desired>::ID() )
         throw CFCast_Failed();
      
      return static_cast< Desired >( p.Get() );
     }

   template < class Desired > Owned<Desired> CFCast( Owned<CFTypeRef> p )
     {
      Desired result = CFCast<Desired>( p.get() );
      p.release();
      return Owned<Desired>::Seize( result );
     }
  }

#endif
