/*
	cursor_invisible.hh
	-------------------
*/

#ifndef FREND_CURSORINVISIBLE_HH
#define FREND_CURSORINVISIBLE_HH

// v68k-cursor
#include "cursor/cursor.hh"

// frontend-common
#include "frend/cursor.hh"


namespace frend
{

inline
bool cursor_invisible()
{
	/*
		Returns true if a hardware cursor is present
		and currently not visible; false otherwise.
	*/
	
	return cursor_state  &&  ! cursor_state->visible;
}

}

#endif
