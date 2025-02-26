/*
	transcode.hh
	------------
*/

#ifndef CALLOUT_TRANSCODE_HH
#define CALLOUT_TRANSCODE_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t transcode_8x_1bpp_to_8bpp_callout( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
