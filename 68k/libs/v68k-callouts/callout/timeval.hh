/*
	timeval.hh
	----------
*/

#ifndef CALLOUT_TIMEVAL_HH
#define CALLOUT_TIMEVAL_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t timeval_from_microseconds_callout( v68k::processor_state& s );
int32_t timeval_from_nanoseconds_callout ( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
