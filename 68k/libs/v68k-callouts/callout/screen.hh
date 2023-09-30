/*
	screen.hh
	---------
*/

#ifndef CALLOUT_SCREEN_HH
#define CALLOUT_SCREEN_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t ScrnBitMap_callout   ( v68k::processor_state& s );
int32_t lock_screen_callout  ( v68k::processor_state& s );
int32_t unlock_screen_callout( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
