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


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k {
namespace mac  {

uint32_t get_Ticks()
{
	using namespace v68k::time;
	
	const uint64_t delta = guest_uptime_microseconds();
	
	const unsigned microseconds_per_tick = 1000 * 1000 * 100 / 6015;
	
	return delta / microseconds_per_tick;
}

uint32_t get_Time()
{
	return ::time( NULL ) + ::mac::types::epoch_delta();  // TODO:  Local time
}

}  // namespace mac
}  // namespace v68k
