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
	
	using Nitrogen::CFType_Traits;
	
	
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
	
}

#endif

