/*
	clock.cc
	--------
*/

#include "clock.hh"

// iota
#include "iota/endian.hh"

// v68k-record
#include "record/mb32.hh"
#include "record/recording.hh"
#include "record/time.hh"


namespace v68k   {
namespace record {

struct clock_chunk
{
	uint32_t tag;
	uint32_t size;
	uint32_t clock;
	uint32_t value;
};

void clock_Time( unsigned long Time )
{
	if ( ! recording() )
	{
		return;
	}
	
	timestamp();
	
	clock_chunk clock;
	
	clock.tag   = mb32( 'C', 'L', 'O', 'K' );
	clock.size  = iota::big_u32( sizeof clock );
	clock.clock = mb32( 'T', 'i', 'm', 'e' );
	clock.value = iota::big_u32( Time );
	
	append( &clock, sizeof clock );
}

}  // namespace record
}  // namespace v68k
