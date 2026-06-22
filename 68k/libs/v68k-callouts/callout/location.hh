/*
	location.hh
	-----------
*/

#ifndef CALLOUT_LOCATION_HH
#define CALLOUT_LOCATION_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t ReadLocation_callout( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
