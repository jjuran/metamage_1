/*
	notify.cc
	---------
*/

#include "Genie/notify.hh"

// Standard C
#include <signal.h>
#include <stdlib.h>

// debug
#include "debug/assert.hh"


namespace Genie
{
	
	static
	void sighup( int )
	{
		// empty
	}
	
	void install_empty_sighup_handler()
	{
		struct sigaction sig;
		sig.sa_handler = &sighup;
		sig.sa_mask    = 0;
		sig.sa_flags   = 0;
		
		int nok = sigaction( SIGHUP, &sig, NULL );
		
		if ( nok )
		{
			abort();
		}
	}
	
	static
	bool verify_empty_sighup_handler()
	{
		struct sigaction sig;
		
		int nok = sigaction( SIGHUP, NULL, &sig );
		
		if ( nok )
		{
			abort();
		}
		
		return sig.sa_handler == &sighup;
	}
	
	int notify_thread_via_sighup( pthread_t thread )
	{
		ASSERT( verify_empty_sighup_handler() );
		
		return pthread_kill( thread, SIGHUP );
	}
	
}
