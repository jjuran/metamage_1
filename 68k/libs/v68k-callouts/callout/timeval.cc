/*
	timeval.cc
	----------
*/

#include "callout/timeval.hh"

// v68k
#include "v68k/endian.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {

enum
{
	rts = 0x4E75,
};

static
bool get_quad( v68k::processor_state& s, uint32_t addr, uint64_t& result )
{
	const v68k::function_code_t data_space = s.data_space();
	
	return s.get_long( addr,     high_long( result ), data_space )  &&
	       s.get_long( addr + 4, low_long ( result ), data_space );
}

static
bool put_timeval( v68k::processor_state& s, uint32_t addr, uint64_t useconds )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint32_t  sec = useconds / 1000000;
	uint32_t usec = useconds % 1000000;
	
	return s.put_long( addr,      sec, data_space )  &&
	       s.put_long( addr + 4, usec, data_space );
}

int32_t timeval_from_microseconds_callout( v68k::processor_state& s )
{
	const uint32_t tv = s.a(0);
	const uint32_t us = s.a(1);
	
	uint64_t microseconds;
	
	bool ok = get_quad   ( s, us, microseconds )  &&
	          put_timeval( s, tv, microseconds );
	
	return ok ? rts : (int) v68k::Bus_error;
}

int32_t timeval_from_nanoseconds_callout( v68k::processor_state& s )
{
	const uint32_t tv = s.a(0);
	const uint32_t ns = s.a(1);
	
	uint64_t nanoseconds;
	
	bool ok = get_quad   ( s, ns, nanoseconds )  &&
	          put_timeval( s, tv, nanoseconds / 1000 );
	
	return ok ? rts : (int) v68k::Bus_error;
}

}  // namespace callout
}  // namespace v68k
