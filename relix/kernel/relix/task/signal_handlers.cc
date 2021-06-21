/*
	signal_handlers.cc
	------------------
*/

#include "relix/task/signal_handlers.hh"

// Standard C
#include <string.h>

// POSIX
#include <signal.h>

// more-libc
#include "more/string.h"


namespace relix
{
	
	class signal_handlers_impl : public signal_handlers
	{
		public:
			struct sigaction its_actions[ NSIG ];
		
		public:
			signal_handlers_impl();
			
			signal_handlers_impl( const signal_handlers_impl& other );
			
			friend void swap( signal_handlers_impl& a, signal_handlers_impl& b );
	};
	
	signal_handlers_impl::signal_handlers_impl()
	{
		memset( its_actions, 0, sizeof its_actions );
	}
	
	signal_handlers_impl::signal_handlers_impl( const signal_handlers_impl& other )
	{
		mempcpy( its_actions, other.its_actions, sizeof its_actions );
	}
	
	void swap( signal_handlers_impl& a, signal_handlers_impl& b )
	{
		struct sigaction temp_actions[ NSIG ];
		
		mempcpy( temp_actions,  a.its_actions, sizeof temp_actions );
		mempcpy( a.its_actions, b.its_actions, sizeof temp_actions );
		mempcpy( b.its_actions, temp_actions,  sizeof temp_actions );
	}
	
	
	static inline const struct sigaction* get_actions( const signal_handlers* that )
	{
		return static_cast< const signal_handlers_impl* >( that )->its_actions;
	}
	
	static inline struct sigaction* get_actions( signal_handlers* that )
	{
		return static_cast< signal_handlers_impl* >( that )->its_actions;
	}
	
	void signal_handlers::set( int signo, const struct sigaction& action )
	{
		get_actions( this )[ signo ] = action;
	}
	
	const struct sigaction& signal_handlers::signal_handlers::get( int signo ) const
	{
		return get_actions( this )[ signo ];
	}
	
	void signal_handlers::reset_handlers()
	{
		struct sigaction* actions = get_actions( this );
		
		for ( int i = 0;  i < NSIG;  ++i )
		{
			if ( actions[ i ].sa_handler != SIG_IGN )
			{
				actions[ i ].sa_handler = SIG_DFL;
			}
		}
	}
	
	void signal_handlers::swap( signal_handlers& other )
	{
		relix::swap( static_cast< signal_handlers_impl& >( *this ),
		             static_cast< signal_handlers_impl& >( other ) );
	}
	
	signal_handlers* signal_handlers::create()
	{
		return new signal_handlers_impl();
	}
	
	signal_handlers* duplicate( const signal_handlers& one )
	{
		return new signal_handlers_impl( static_cast< const signal_handlers_impl& >( one ) );
	}
	
	void destroy( const signal_handlers* x )
	{
		delete static_cast< const signal_handlers_impl* >( x );
	}
	
}
