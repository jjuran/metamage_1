/*
	raster_lock.cc
	--------------
*/

#include "raster_lock.hh"

// ams-common
#include "cursor_jump.hh"
#include "screen_lock.hh"


raster_lock::raster_lock( bool cursor, bool screen )
:
	cursor_locked( cursor ),
	screen_locked( screen )
{
	if ( screen )
	{
		lock_screen();
	}
	
	if ( cursor )
	{
		JHideCursor();
	}
}

raster_lock::~raster_lock()
{
	if ( cursor_locked )
	{
		JShowCursor();
	}
	
	if ( screen_locked )
	{
		unlock_screen();
	}
}
