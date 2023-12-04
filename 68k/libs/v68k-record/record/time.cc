/*
	time.cc
	-------
*/

#include "time.hh"

// POSIX
#include <sys/time.h>

// compat
#include "clock/time.h"

// iota
#include "iota/endian.hh"

// v68k-record
#include "record/mb32.hh"
#include "record/recording.hh"


namespace v68k   {
namespace record {

struct time_chunk
{
	uint32_t tag;
	uint32_t size;
	uint32_t time;
};

static
uint64_t microseconds_time()
{
	timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	
	return ts.tv_sec * 1000000ull + ts.tv_nsec / 1000u;
}

void timestamp()
{
	static uint64_t prev_time = microseconds_time();
	
	const uint64_t now = microseconds_time();
	
	if ( now != prev_time )
	{
		time_chunk time;
		
		time.tag  = mb32( 'T', 'I', 'M', 'E' );
		time.size = iota::big_u32( sizeof time );
		time.time = iota::big_u32( now - prev_time );
		
		append( &time, sizeof time );
		
		prev_time = now;
	}
}

}  // namespace record
}  // namespace v68k
