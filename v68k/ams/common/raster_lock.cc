/*
	raster_lock.cc
	--------------
*/

#include "raster_lock.hh"

// ams-common
#include "cursor_jump.hh"
#include "screen_lock.hh"


raster_lock::raster_lock( bool cond ) : condition( cond )
{
	if ( cond )
	{
		lock_screen();
		JHideCursor();
	}
}

raster_lock::~raster_lock()
{
	if ( condition )
	{
		JShowCursor();
		unlock_screen();
	}
}
