/*
	OSUtils.cc
	----------
*/

#include "OSUtils.hh"

// POSIX
#include <time.h>
#include <unistd.h>


pascal long Delay_patch( long numTicks : __A0 ) : __D0
{
	const long nanoseconds_per_tick = 1000 * 1000 * 1000 / 60;
	
	const long seconds     = numTicks / 60;
	const long nanoseconds = numTicks % 60 * nanoseconds_per_tick;
	
	const timespec delay = { seconds, nanoseconds };
	
	timespec remaining;
	
	int woken = nanosleep( &delay, &remaining );
	
	if ( woken )
	{
		// FIXME:  Return Ticks
	}
	
	return 0;
}

pascal void SysBeep_patch( short duration )
{
	char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
}

