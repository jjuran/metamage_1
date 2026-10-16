/*
	cursor.cc
	---------
*/

#include "frend/cursor.hh"


namespace frend
{

bool cursor_ejected;
bool cursor_pinned;
bool cursor_to_be_pinned;

const shared_cursor_state* cursor_state;

}
