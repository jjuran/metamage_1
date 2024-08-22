/*
	codec.hh
	--------
*/

#ifndef CALLOUT_CODEC_HH
#define CALLOUT_CODEC_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t decode_MACE3_callout( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
