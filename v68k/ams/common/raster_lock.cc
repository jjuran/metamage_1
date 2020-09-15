/*
	raster_lock.cc
	--------------
*/

#include "raster_lock.hh"

// ams-common
#include "cursor_jump.hh"
#include "screen_lock.hh"


void lock_raster()
{
	lock_screen();
	JHideCursor();
}

void unlock_raster()
{
	JShowCursor();
	unlock_screen();
}
