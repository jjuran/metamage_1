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

// v68k-record
#include "record/clock.hh"


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
	using mac::types::epoch_delta;
	
	static uint32_t prev_Time;
	
	uint32_t this_Time = ::time( NULL ) + epoch_delta();  // TODO:  Local time
	
	if ( this_Time != prev_Time )
	{
		prev_Time = this_Time;
		
		v68k::record::clock_Time( this_Time );
	}
	
	return this_Time;
}

}  // namespace lowmem
}  // namespace v68k
