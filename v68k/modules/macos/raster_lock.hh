/*
	raster_lock.hh
	--------------
*/

#ifndef RASTERLOCK_HH
#define RASTERLOCK_HH

// macos
#include "Cursor.hh"
#include "screen_lock.hh"


class raster_lock
{
	private:
		screen_lock  locked_screen;
		cursor_lock  locked_cursor;
		
		// non-copyable
		raster_lock           ( const raster_lock& );
		raster_lock& operator=( const raster_lock& );
	
	public:
		raster_lock()
		{
		}
		
		~raster_lock()
		{
		}
};

#endif
