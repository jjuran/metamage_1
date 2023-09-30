/*
	notify_cursor.hh
	----------------
*/

#ifndef CALLOUT_NOTIFYCURSOR_HH
#define CALLOUT_NOTIFYCURSOR_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t notify_cursor_moved_callout( v68k::processor_state& s );
int32_t notify_cursor_vis_callout  ( v68k::processor_state& s );
int32_t notify_cursor_set_callout  ( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k

#endif
