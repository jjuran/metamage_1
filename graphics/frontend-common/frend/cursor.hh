/*
	cursor.hh
	---------
*/

#ifndef FREND_CURSOR_HH
#define FREND_CURSOR_HH

// v68k-cursor
#include "cursor/cursor_fwd.hh"


namespace frend
{

using v68k::cursor::shared_cursor_state;


/*
	We generally interpret an invisible cursor as a
	cue to pin the cursor to the window.  At launch,
	though, the guest cursor is initially invisible
	(giving the back end a chance to set the cursor's
	image before making it visible), while the front
	end regards the cursor as unpinned by default.
	This discrepancy can cause trouble unless we
	specifically deal with it.
	
	If hide-cursor is active in the settings, then
	cursor_to_be_pinned will be set.  This is our
	cue to pin the cursor to the window immediately.
*/

extern bool cursor_ejected;
extern bool cursor_pinned;
extern bool cursor_to_be_pinned;

extern const shared_cursor_state* cursor_state;

}

#endif
