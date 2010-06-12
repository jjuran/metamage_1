/*
	Carbon/CF/Types/CFTypeRef.hh
	----------------------------
*/

#ifndef CARBON_CF_TYPES_CFTYPEREF_HH
#define CARBON_CF_TYPES_CFTYPEREF_HH

#ifndef __CFBASE__
#include <CFBase.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/disposer.hh"


namespace Nitrogen
{
	
	template < class T > struct CFType_Traits;
	
}

namespace Carbon
{
	
	enum CFTypeID
	{
		kCFTypeID_Max = nucleus::enumeration_traits< ::CFTypeID >::max
	};
	
	
	using Nitrogen::CFType_Traits;
	
	template < class CF, ::CFTypeID (*getTypeID)() >
	struct Basic_CFType_Traits
	{
		static CFTypeID ID()                                 { return CFTypeID( getTypeID() ); }
		static ::CFTypeRef ConvertToCFTypeRef( CF value )    { return value; }
	};
	
	
	class CFTypeRef
	{
		private:
			::CFTypeRef value;
		
		public:
			CFTypeRef()                   : value( 0 ) {}
			CFTypeRef( ::CFTypeRef v )    : value( v ) {}
			
			template < class T >
			CFTypeRef( T* v )             : value( CFType_Traits< T* >::ConvertToCFTypeRef( v ) ) {}
			
			static CFTypeRef Make( ::CFTypeRef v )                                { return CFTypeRef( v ); }
			::CFTypeRef Get() const                                               { return value; }
			operator ::CFTypeRef() const                                          { return value; }
			
			friend bool operator==( const CFTypeRef& a, const CFTypeRef& b )      { return a.Get() == b.Get(); }
			friend bool operator!=( const CFTypeRef& a, const CFTypeRef& b )      { return a.Get() != b.Get(); }
			
			friend bool operator==( const CFTypeRef& a, const ::CFTypeRef& b )    { return a.Get() == b; }
			friend bool operator!=( const CFTypeRef& a, const ::CFTypeRef& b )    { return a.Get() != b; }
			
			friend bool operator==( const ::CFTypeRef& a, const CFTypeRef& b )    { return a == b.Get(); }
			friend bool operator!=( const ::CFTypeRef& a, const CFTypeRef& b )    { return a != b.Get(); }
	};
	
}

namespace Nitrogen
{
	
	using Carbon::Basic_CFType_Traits;
	
	template <> struct CFType_Traits< CFStringRef    > : Basic_CFType_Traits< CFStringRef,    ::CFStringGetTypeID    > {};
	template <> struct CFType_Traits< CFAllocatorRef > : Basic_CFType_Traits< CFAllocatorRef, ::CFAllocatorGetTypeID > {};
	
}

namespace nucleus
{
	
	template <> struct disposer< Carbon::CFTypeRef >
	{
		typedef Carbon::CFTypeRef  argument_type;
		typedef void               result_type;
		
		void operator()( Carbon::CFTypeRef cf ) const
		{
			::CFRelease( cf );
		}
	};
	
	template <> struct disposer_class< CFStringRef >   : disposer_class< Carbon::CFTypeRef > {};
	template <> struct disposer_class< CFAllocatorRef >: disposer_class< Carbon::CFTypeRef > {};
	
}

#endif

