/*
	memwatch.cc
	-----------
*/

#include "memwatch.hh"

// POSIX
#include <sys/time.h>

// ams-core
#include "reactor-core.hh"


void WaitUntilZero_patch( unsigned short       trap : __D1,
                          const volatile char* addr : __A0 )
{
	timeval timeout;
	
	timeout.tv_usec = 0;
	
again:
	
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
	
	if ( trap & 0x0200 )
	{
		/*
			Wait for the word to reach zero, not just a byte.
			
			We'll assume that this is a counter that counts down to zero,
			so atomicity isn't an issue:  The high byte will reach zero
			and stay there first, followed by the low byte.
		*/
		
		trap &= ~0x0200;
		
		++addr;
		
		goto again;
	}
}
