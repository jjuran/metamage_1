/*
	uptime.hh
	---------
*/

#ifndef CALLOUT_UPTIME_HH
#define CALLOUT_UPTIME_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t get_Ticks_callout       ( v68k::processor_state& s );
int32_t get_microseconds_callout( v68k::processor_state& s );
int32_t microseconds_callout    ( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
