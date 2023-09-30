/*
	uptime.cc
	---------
*/

#include "callout/uptime.hh"

// v68k
#include "v68k/endian.hh"

// v68k-time
#include "v68k-time/clock.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {


enum
{
	rts = 0x4E75,
};

int32_t get_Ticks_callout( v68k::processor_state& s )
{
	using namespace v68k::time;
	
	s.d(0) = guest_uptime_ticks();
	
	return rts;
}

int32_t get_microseconds_callout( v68k::processor_state& s )
{
	using namespace v68k::time;
	
	const uint64_t t = guest_uptime_microseconds();
	
	/*
		_Microseconds expects the low long in D0 and the high long in A0.
	*/
	
	s.a(0) = uint32_t( t >> 32 );
	s.d(0) = uint32_t( t       );
	
	return rts;
}

int32_t microseconds_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t sp = s.a(7);
	
	uint32_t result_address;
	
	if ( ! s.get_long( sp + 4, result_address, data_space ) )
	{
		return v68k::Bus_error;
	}
	
	using namespace v68k::time;
	
	uint64_t t = guest_uptime_microseconds();
	
	if ( ! s.put_long( result_address, high_long( t ), data_space ) )
	{
		return v68k::Bus_error;
	}
	
	if ( ! s.put_long( result_address + 4, low_long( t ), data_space ) )
	{
		return v68k::Bus_error;
	}
	
	return rts;
}

}  // namespace callout
}  // namespace v68k
