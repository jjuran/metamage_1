/*
	memwatch.cc
	-----------
*/

#include "memwatch.hh"

// POSIX
#include <sys/time.h>

// ams-core
#include "reactor-core.hh"


void WaitUntilZero_patch( const volatile short* addr : __A0 )
{
	timeval timeout;
	
	timeout.tv_usec = 0;
	
	while ( *addr != 0 )
	{
		/*
			The timeout is arbitrary, since reactor_wait() will return
			every time something it's waiting for happens.  But we have
			to reset it each time.
		*/
		
		timeout.tv_sec = 127;
		
		reactor_wait( &timeout );
	}
}
