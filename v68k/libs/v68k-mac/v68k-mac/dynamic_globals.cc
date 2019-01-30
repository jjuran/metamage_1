/*
	dynamic_globals.cc
	------------------
*/

#include "v68k-mac/dynamic_globals.hh"

// POSIX
#include <sys/time.h>

// Standard C
#include <time.h>

// mac-types
#include "mac_types/epoch.hh"

// v68k-time
#include "v68k-time/clock.hh"


#pragma exceptions off


namespace v68k {
namespace mac  {

uint32_t get_Ticks()
{
	using namespace v68k::time;
	
	static uint32_t Ticks = 0;
	
	const uint64_t delta = microsecond_clock() - initial_clock;
	
	const unsigned microseconds_per_tick = 1000 * 1000 * 100 / 6015;
	
	uint32_t ticks = delta / microseconds_per_tick;
	
	if ( ticks == Ticks )
	{
		const uint32_t wakeup = ++ticks * 1ull * microseconds_per_tick - delta;
		
		timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = wakeup * 1000;
		
		nanosleep( &ts, NULL );
		
		return ticks;
	}
	
	return Ticks = ticks;
}

uint32_t get_Time()
{
	return ::time( NULL ) + ::mac::types::epoch_delta();  // TODO:  Local time
}

}  // namespace mac
}  // namespace v68k
