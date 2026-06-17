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


extern bool cursor_pinned;

extern const shared_cursor_state* cursor_state;

}

#endif
