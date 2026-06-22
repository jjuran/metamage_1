/*
	dynamic_globals.cc
	------------------
*/

#include "lowmem/dynamic_globals.hh"

// Mac OS
#ifdef __MACOS__
#if TARGET_API_MAC_CARBON
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif
#endif
#endif

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


#define LOWMEM( addr, type )  (*(type*) (addr))

#define Time  LOWMEM( 0x020C, uint32_t )


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
	
#ifdef __MACOS__
#if TARGET_API_MAC_CARBON
	
	#undef Time
	
	unsigned long Time;
	
	GetDateTime( &Time );
	
#endif
	
	return Time;
	
#endif
	
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
