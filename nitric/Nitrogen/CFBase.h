// CFBase.h

#ifndef NITROGEN_CFBASE_H
#define NITROGEN_CFBASE_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFBASE__
#include FRAMEWORK_HEADER(CoreFoundation,CFBase.h)
#endif
#ifndef __CFSTRING__
#include FRAMEWORK_HEADER(CoreFoundation,CFString.h)
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

namespace Nitrogen
  {
   typedef Nucleus::ID< class CFTypeID_Tag, ::CFTypeID >::Type CFTypeID;

   template < class T > struct CFType_Traits;
   
   template < class CF, ::CFTypeID (*getTypeID)() >
   struct Basic_CFType_Traits
     {
      static CFTypeID ID()                                    { return CFTypeID( getTypeID() ); }
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

}

namespace Nucleus {
   template <> struct Disposer<Nitrogen::CFTypeRef>: public std::unary_function< Nitrogen::CFTypeRef, void >
     {
      void operator()( Nitrogen::CFTypeRef cf ) const
        {
         ::CFRelease( cf );
        }
     };
   
}

namespace Nitrogen {
   template < class T > bool operator==( const CFTypeRef& a, T const *const& b )   { return a == CFTypeRef( b ); }
   template < class T > bool operator!=( const CFTypeRef& a, T const *const& b )   { return a != CFTypeRef( b ); }
   
   template < class T > bool operator==( T const *const& a, const CFTypeRef& b )   { return CFTypeRef( a ) == b; }
   template < class T > bool operator!=( T const *const& a, const CFTypeRef& b )   { return CFTypeRef( a ) != b; }

   typedef Nucleus::Flag< class CFOptionFlags_Tag, ::CFOptionFlags >::Type CFOptionFlags;
   
   NUCLEUS_DEFINE_FLAG_OPS( CFOptionFlags )
   
   typedef Nucleus::ID< class CFHashCode_Tag, ::CFHashCode >::Type CFHashCode;
   
   using ::CFStringRef;
   using ::CFAllocatorRef;
   
   template <> struct CFType_Traits< CFStringRef >: Basic_CFType_Traits< CFStringRef, ::CFStringGetTypeID > {};
   template <> struct CFType_Traits< CFAllocatorRef >: Basic_CFType_Traits< CFAllocatorRef, ::CFAllocatorGetTypeID > {};
}

namespace Nucleus {
   template <> struct OwnedDefaults< Nitrogen::CFStringRef >   : OwnedDefaults< Nitrogen::CFTypeRef > {};
   template <> struct OwnedDefaults< Nitrogen::CFAllocatorRef >: OwnedDefaults< Nitrogen::CFTypeRef > {};
}

namespace Nitrogen {

   inline CFTypeID CFGetTypeID( CFTypeRef cf )
     {
      return CFTypeID( ::CFGetTypeID( cf ) );
     }

   inline Nucleus::Owned<CFStringRef> CFCopyTypeIDDescription( CFTypeID cf )
     {
      return Nucleus::Owned<CFStringRef>::Seize( ::CFCopyTypeIDDescription( cf ) );
     }
   
   template < class CF >
   Nucleus::Owned< CF > CFRetain( CF toRetain )
     {
      ::CFRetain( CFTypeRef( toRetain ) );
      return Nucleus::Owned<CF>::Seize( toRetain );
     }

   inline void CFRelease( Nucleus::Owned<CFTypeRef> /*toRelease*/ )
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
      return CFHashCode( ::CFHash( cf ) );
     }

   inline Nucleus::Owned<CFStringRef> CFCopyDescription( CFTypeRef cf )
     {
      return Nucleus::Owned<CFStringRef>::Seize( ::CFCopyDescription( cf ) );
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

   template < class Desired > Nucleus::Owned<Desired> CFCast( Nucleus::Owned<CFTypeRef> p )
     {
      Desired result = CFCast<Desired>( p.get() );
      p.release();
      return Nucleus::Owned<Desired>::Seize( result );
     }
  }

#endif
