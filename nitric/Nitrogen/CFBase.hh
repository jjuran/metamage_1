// Nitrogen/CFBase.hh
// ------------------

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFBASE_HH
#define NITROGEN_CFBASE_HH

#ifndef __CFBASE__
#include <CFBase.h>
#endif
#ifndef __CFSTRING__
#include <CFString.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"
#include "nucleus/owned.hh"

namespace Nitrogen
  {
	
	enum CFTypeID
	{
		kCFTypeID_Max = nucleus::enumeration_traits< ::CFTypeID >::max
	};
	
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

namespace nucleus {
   template <> struct disposer<Nitrogen::CFTypeRef>: public std::unary_function< Nitrogen::CFTypeRef, void >
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

	enum CFOptionFlags
	{
		kCFOptionFlags_Max = nucleus::enumeration_traits< ::CFOptionFlags >::max
	};
	
   NUCLEUS_DEFINE_FLAG_OPS( CFOptionFlags )
   
	enum CFHashCode
	{
		kCFHashCode_Max = nucleus::enumeration_traits< ::CFHashCode >::max
	};
	
   using ::CFStringRef;
   using ::CFAllocatorRef;
   
   template <> struct CFType_Traits< CFStringRef >: Basic_CFType_Traits< CFStringRef, ::CFStringGetTypeID > {};
   template <> struct CFType_Traits< CFAllocatorRef >: Basic_CFType_Traits< CFAllocatorRef, ::CFAllocatorGetTypeID > {};
}

namespace nucleus {
   template <> struct disposer_traits< Nitrogen::CFStringRef >   : disposer_traits< Nitrogen::CFTypeRef > {};
   template <> struct disposer_traits< Nitrogen::CFAllocatorRef >: disposer_traits< Nitrogen::CFTypeRef > {};
}

namespace Nitrogen {

   inline CFTypeID CFGetTypeID( CFTypeRef cf )
     {
      return CFTypeID( ::CFGetTypeID( cf ) );
     }

   inline nucleus::owned<CFStringRef> CFCopyTypeIDDescription( CFTypeID cf )
     {
      return nucleus::owned<CFStringRef>::seize( ::CFCopyTypeIDDescription( cf ) );
     }
   
   template < class CF >
   nucleus::owned< CF > CFRetain( CF toRetain )
     {
      ::CFRetain( CFTypeRef( toRetain ) );
      return nucleus::owned<CF>::seize( toRetain );
     }

   inline void CFRelease( nucleus::owned<CFTypeRef> /*toRelease*/ )
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

   inline nucleus::owned<CFStringRef> CFCopyDescription( CFTypeRef cf )
     {
      return nucleus::owned<CFStringRef>::seize( ::CFCopyDescription( cf ) );
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

   template < class Desired > nucleus::owned<Desired> CFCast( nucleus::owned<CFTypeRef> p )
     {
      Desired result = CFCast<Desired>( p.get() );
      p.release();
      return nucleus::owned<Desired>::seize( result );
     }
  }

#endif
