/*
	OSUtils.cc
	----------
*/

#include "OSUtils.hh"

// POSIX
#include <time.h>
#include <unistd.h>


uint32_t Ticks : 0x016A;


pascal long Delay_patch( long numTicks : __A0 ) : __D0
{
	const long nanoseconds_per_tick = 1000 * 1000 * 1000 / 60;
	
	const long seconds     = numTicks / 60;
	const long nanoseconds = numTicks % 60 * nanoseconds_per_tick;
	
	timespec delay = { seconds, nanoseconds };
	
	timespec remaining;
	
	while ( nanosleep( &delay, &remaining ) != 0 )
	{
		delay = remaining;
	}
	
	return Ticks;
}

pascal void SysBeep_patch( short duration )
{
	char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
}

