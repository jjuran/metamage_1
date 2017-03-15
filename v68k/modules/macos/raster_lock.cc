/*
	raster_lock.cc
	--------------
*/

#include "raster_lock.hh"

// macos
#include "Cursor.hh"
#include "screen_lock.hh"


void lock_raster()
{
	lock_screen();
	hide_cursor();
}

void unlock_raster()
{
	show_cursor();
	unlock_screen();
}
