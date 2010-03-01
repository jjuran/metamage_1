// nucleus/shared.hh
// -----------------

// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_SHARED_HH
#define NUCLEUS_SHARED_HH

// Nucleus
#include "nucleus/owned.hh"


namespace nucleus
{
	
	class share
	{
		private:
			mutable const share *previous;
			mutable const share *next;
		
		public:
			share()
			:
				previous( this ),
				next    ( this )
			{
			}
			
			share( const share& b )
			:
				previous( &b ),
				next( b.next )
			{
				previous->next = this;
				next->previous = this;
			}
			
			~share()
			{
				previous->next = next;
				next->previous = previous;
			}
			
			share& operator=( const share& b )
			{
				previous->next = next;
				next->previous = previous;
				
				next = b.next;
				previous = next->previous;
				
				previous->next = this;
				next->previous = this;
				
				return *this;
			}
			
			bool sole() const
			{
				return previous == this;
			}
	};
	
	template < class Resource, class Disposer >
	class shared;
	
	/*
		CodeWarrior 7 has bugs preventing a shared< R1, D > from accessing
		the members of shared< R2, D >.  This hack gets around those bugs.
	*/
	
	class shared_access_hack
	{
		protected:
			template < class R, class D >
			const disposable_resource< R, D >& get_body( const shared< R, D >& s )
			{
				return s.its_body;
			}
			
			template < class R, class D >
			const share& get_share( const shared< R, D >& s )
			{
				return s.its_share;
			}
	};

	template < class Resource,
	           class Disposer = typename disposer_traits< Resource >::type >
	class shared: private shared_access_hack
	{
		private:
			friend class shared_access_hack;
			
			typedef Resource resource_type;
			typedef Disposer disposer_type;
			typedef disposable_resource< Resource, Disposer > body_type;
			
			body_type  its_body;
			share      its_share;
		
		public:
			shared() : its_body()
			{
			}
			
			template < class R >
			shared( owned< R, Disposer > s ) : its_body( s.release_body() )
			{
			}
			
			template < class R >
			shared( const shared< R, Disposer >& s )
			:
				its_body ( get_body ( s ) ),
				its_share( get_share( s ) )
			{
			}
			
			// Plus the implicit copy constructor
			
			~shared()
			{
				if ( its_share.sole() )
				{
					its_body.dispose();
				}
			}
			
			shared& operator=( const shared& s )
			{
				shared empty;
				
				swap( empty );
				
				its_body  = get_body ( s );
				its_share = get_share( s );
				
				return *this;
			}
			
			template < class R >
			shared& operator=( const shared< R, Disposer >& s )
			{
				shared empty;
				
				swap( empty );
				
				its_body  = get_body ( s );
				its_share = get_share( s );
				
				return *this;
			}
			
			void swap( shared& s )
			{
				std::swap( its_body,  s.its_body  );
				std::swap( its_share, s.its_share );
			}
			
			const resource_type& get() const       { return its_body.get(); }
			operator const resource_type&() const  { return get(); }
			
			bool sole() const                      { return its_share.sole(); }
			
			void reset()
			{
				shared empty;
				
				swap( empty );
			}
			
			owned< Resource, Disposer > unshare();
	};

	class unshare_failed {};

	template < class Resource, class Disposer >
	owned< Resource, Disposer > shared< Resource, Disposer >::unshare()
	{
		if ( !sole() )
		{
			throw unshare_failed();
		}
		
		body_type released;
		
		released.swap( its_body );
		
		return owned< Resource, Disposer >::seize( released );
	}
	
}

#endif

