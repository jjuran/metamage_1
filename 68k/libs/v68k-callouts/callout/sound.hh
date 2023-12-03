/*
	sound.hh
	--------
*/

#ifndef CALLOUT_SOUND_HH
#define CALLOUT_SOUND_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t send_sound_command_callout( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
