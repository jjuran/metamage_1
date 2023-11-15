/*
	dynamic_globals.cc
	------------------
*/

#include "lowmem/dynamic_globals.hh"

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


namespace v68k   {
namespace lowmem {

uint32_t get_Ticks()
{
	using namespace v68k::time;
	
	return guest_uptime_ticks();
}

uint32_t get_Time()
{
	return ::time( NULL ) + ::mac::types::epoch_delta();  // TODO:  Local time
}

}  // namespace lowmem
}  // namespace v68k
