/*
	Timer.cc
	--------
*/

#include "varyx/mac/Timer.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/integer.hh"


#if TARGET_CPU_68K
#define EXISTS(name)  ::mac::sys::trap_available(_##name)
#else
#define EXISTS(name)  true
#endif

enum
{
	_Microseconds = 0xA193,
};


namespace varyx
{
namespace mac
{

using namespace vlib;

static
Value v_Microseconds( const Value& v )
{
	UnsignedWide result;
	Microseconds( &result );
	
	return Integer( (uint64_t&) result );
}

#define V(name) #name, (EXISTS(name) ? &v_##name : NULL)

const proc_info proc_Microseconds = { V(Microseconds), &empty_list };

}
}
