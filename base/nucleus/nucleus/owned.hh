// nucleus/owned.hh
// ----------------

// Written 2002-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


/*
	owned: Ownership of a disposable resource.
	
	template < class Resource, class Disposer = owned_defaults< Resource >::disposer_type >
	class owned;
	
		Resource: a type which can be disposed, deleted, removed, released, closed, or unlocked
		Disposer: a functor class specifying the manner of disposal
	
	An object of type owned< Resource, Disposer > represents the ownership of, and responsibility
	for disposal of that aspect of Resource which is destroyed by Disposer.  In most cases, the
	aspect destroyed is the entire resource.
	
	The template owned<> is similar to std::auto_ptr, but where auto_ptr uses delete to destroy
	objects created by new-expressions, owned<> uses its Disposer class to destroy resources created
	by other means.
	
	So, for example, owned< WindowRef > represents ownership of a window, and the consequent
	responsibility for calling DisposeWindow.  And owned< CFStringRef > represents ownership of
	(a reference counting point of) a CFString, and a responsibility for calling CFRelease.
	
	The static member function owned< Resource, Disposer >::seize( const Resource& ) can be
	used to claim ownership of a resource.  Conversely, owned< Resource, Disposer >::release()
	is used to give up ownership.  These functions are intended to be used as an interface
	between code using owned<> and code which represents ownership by other means, or which does
	not represent ownership at all.
	
	owned< Resource, Disposer > provides pointer-like operators * and -> which can be used
	to access the owned resource.
	
	The destructor of owned< Resource, Disposer > calls Disposer::operator() to dispose of the
	owned resource.  Since default-constructed objects of type owned< Resource, Disposer >
	own an explicitly default-constructed Resource, it is important for Disposer::operator()
	to do little or nothing when passed this value.
	
	Copy construction, assignment, and conversion of owned-types represent transfers of ownership:
	Given objects r1 and r2 of type owned< R, D >, we have:
	
		owned< R, D > r = r1;  // r now holds the resource formerly in r1; r1 holds R().
		
		r = r2;                // the resource formerly in r is disposed;
		                       // r now holds the resource formerly in r2; r2 holds R().
		
		owned< S, D > s = r;   // s now holds the resource formerly in r, converted to type S;
		                       // r holds R().
	
	Conversion from owned< R, DR > to owned< S, DS > is only allowed when R is implicitly convertable
	to S and DR is the same type as DS.
	
	These rules have implications for the use of owned-types in function signatures.  A function
	parameter of type owned< R, D > represents a transfer of ownership from the caller to the called
	function; a function result of type owned< R, D > represents a transfer of ownership from the
	called function to the caller.
	
	It's possible to be caught off guard by an unexpected transfer of ownership out of an
	owned object.  One may guard against this problem by declaring owned objects const where
	transfer is not desired.  Const owned objects hold ownership of their resources for
	their entire lifetime.
	
	Since most types are disposed of in only one sense, owned<> uses a traits class
	
		template < class Resource > struct owned_defaults;
	
	to find the default disposer for each type Resource.  The default disposer is
	owned_defaults< Resource >::disposer_type; in the unspecialized owned_defaults template,
	this is a typedef name for disposer<Resource>.
	
	The template 
	
		template < class Resource > struct disposer;
	
	has no definition; instead, specializations provide the default disposers for various
	types.
	
	This two stage lookup for disposers allows default disposers to be specified in two ways.
	Most often, one specializes disposer to provide the right disposer for a particular type:
	
		namespace nucleus
		{
			template <> struct disposer<CFTypeRef>: public std::unary_function< CFTypeRef, void >
			{
				void operator()( CFTypeRef cf ) const
				{
					if ( cf != 0 )
						::CFRelease( cf );
				}
			};
		}      
	
	But conversions are only allowed when two types share a disposer class.  To acheive this
	effect, specialize owned_defaults:
	
		namespace nucleus
		{      
			template <> struct owned_defaults< CFStringRef > : owned_defaults< CFTypeRef > {};
		}
	
	With these declarations, owned_defaults< CFStringRef >::disposer_type and owned_defaults< CFTypeRef >::disposer_type
	are both names for disposer< CFTypeRef >, and thus owned< CFStringRef > may be converted to
	owned< CFTypeRef >.
*/

#ifndef NUCLEUS_OWNED_HH
#define NUCLEUS_OWNED_HH

// iota
#include "iota/swap.hh"

// nucleus
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif


namespace nucleus
{
	
	template < class Resource >
	struct disposer_traits : disposer_class< Resource >
	{
	};
	
	template < class Resource >
	struct null_resource
	{
		static Resource get()  { return Resource(); }
	};
	
	
	struct nonzero_values_are_live
	{
		template < class Resource >
		static bool is_live( const Resource& r )
		{
			return r != 0;
		}
	};
	
	struct nonzero_pointers_are_live
	{
		static bool is_live( const void* r )
		{
			return r != 0;
		}
	};
	
	template < class Resource, class Disposer >
	struct aliveness_traits
	{
		typedef nonzero_values_are_live aliveness_test;
	};
	
	template < class T, class Disposer >
	struct aliveness_traits< T*, Disposer >
	{
		typedef nonzero_pointers_are_live aliveness_test;
	};
	
	
	template < class Disposer >
	struct disposer_size
	{
		struct derived : Disposer
		{
			int x;
		};
		
		// If zero, the disposer is stateless
		static const unsigned long value = sizeof (derived) - sizeof (int);
	};
	
	
	template < class Resource, class Disposer >
	class disposable_resource : private Disposer
	{
		private:
			typedef Resource resource_type;
			typedef Disposer disposer_type;
			
			resource_type its_resource;
			
			static resource_type implicitly_convert( const resource_type& r )
			{
				return r;
			}
		
		public:
			disposable_resource()
			:
				disposer_type(),
				its_resource( null_resource< Resource >::get() )
			{
			}
			
			disposable_resource( const resource_type& r )
			:
				disposer_type(),
				its_resource( r )
			{
			}
			
			disposable_resource( const Resource& r, const Disposer& disposer )
			:
				disposer_type( disposer ),
				its_resource( r )
			{
			}
			
			template< class R >
			disposable_resource( const disposable_resource< R, Disposer >& r )
			:
				disposer_type( r.disposer() ),
				its_resource( implicitly_convert( r.get() ) )
			{
			}
			
			void swap( disposable_resource& r )
			{
				using iota::swap;
				
				if ( disposer_size< Disposer >::value != 0 )
				{
					swap( disposer(), r.disposer() );
				}
				
				swap( its_resource, r.its_resource );
			}
			
			disposer_type const& disposer() const      { return *this; }
			disposer_type      & disposer()            { return *this; }
			
			bool is_live() const
			{
				typedef aliveness_traits< Resource, Disposer > traits;
				
				return traits::aliveness_test::is_live( get() );
			}
			
			void dispose() const
			{
				if ( is_live() )
				{
					disposer()( its_resource );
				}
			}
			
			const resource_type& get() const           { return its_resource; }
			operator const resource_type&() const      { return its_resource; }
	};
	
	
	template < class Resource, class Disposer >
	class ownership_transfer
	{
		private:
			typedef disposable_resource< Resource, Disposer > body_type;
			
			body_type its_body;
		
		public:
			explicit ownership_transfer( const body_type& source )
			:
				its_body( source )
			{
			}
			
			body_type& operator*()       { return its_body; }
	};
	
	
	template < class Resource,
	           class Disposer = typename disposer_traits< Resource >::type >
	class owned
	{
		private:
			typedef Resource resource_type;
			typedef Disposer disposer_type;
			typedef disposable_resource< Resource, Disposer > body_type;
			typedef ownership_transfer < Resource, Disposer > transfer;
			
			body_type its_body;
			
			class seizing {};
			explicit owned( seizing, const body_type& body ) : its_body( body )
			{
			}
			
			explicit owned( seizing, const resource_type& r ) : its_body( r )
			{
			}
		
		public:                                       
			owned()                 : its_body()  {}
			owned( owned& s )       : its_body()  { swap( s ); }
			owned( transfer s )     : its_body()  { its_body.swap( *s ); }
			
			template< class R >
			owned( owned< R, Disposer >& s ) : its_body( s.release_body() )
			{
			}
			
			~owned()                               { its_body.dispose(); }
			
			// Assignment
			
			owned& operator=( owned& s )
			{
				owned old( *this );
				
				swap( s );
				
				return *this;
			}
			
			owned& operator=( transfer s )
			{
				owned old( *this );
				
				its_body.swap( *s );
				
				return *this;
			}
			
			template< class R > 
			owned& operator=( owned< R, Disposer >& s )
			{
				owned old( *this );
				
				its_body = s.release_body();
				
				return *this;
			}
			
			// Conversion
			
			template < class R >
			operator owned< R, Disposer >()
			{
				return owned< R, Disposer >::seize( release_body() );
			}
			
			template < class R >
			operator ownership_transfer< R, Disposer >()
			{
				return ownership_transfer< R, Disposer >( release_body() );
			}
			
			// Swap
			void swap( owned& s )               { its_body.swap( s.its_body ); }
			
			// Getters
			
			const resource_type& get() const          { return its_body.get(); }
			operator const resource_type&() const     { return get(); }
			
			disposer_type const& disposer() const  { return its_body.disposer(); }
			disposer_type      & disposer()        { return its_body.disposer(); }
			
			body_type release_body()
			{
				body_type released;
				
				released.swap( its_body );
				
				return released;
			}
			
			resource_type release()    { return release_body().get(); }
			
			void reset()               { owned empty;  swap( empty ); }
			
			// Seize
			
			static owned seize( const body_type& body )   { return owned( seizing(), body ); }
			static owned seize( const resource_type& r )  { return owned( seizing(), r ); }
			
			static owned seize( const resource_type& r, const disposer_type& d )
			{
				return seize( body_type( r, d ) );
			}
	};
	
	template < class Resource, class Disposer >
	inline void swap( owned< Resource, Disposer >&  a,
	                  owned< Resource, Disposer >&  b )
	{
		a.swap( b );
	}
	
	
	template < class Resource, class Disposer >
	struct convert_input_traits< owned< Resource, Disposer > >
	{
		typedef Resource converter_input_type;
	};
	
	// This disposer makes owned< Foo * > similar to std::auto_ptr< Foo >.  I
	// generally recommend std::auto_ptr< Foo > instead -- it helps you get
	// along with the rest of the world.  But sometimes uniformity is more
	// important.
	struct dispose_with_delete
	{
		template < class T >
		void operator()( T *p ) const
		{
			delete p;
		}
	};
	
	// This is for arrays.  The whole class is templated because array
	// conversions are bad.
	template < class T >
	struct dispose_with_array_delete
	{
		typedef T*    argument_type;
		typedef void  result_type;
		
		void operator()( T *p ) const
		{
			delete[] p;
		}
	};
	
}

#endif

