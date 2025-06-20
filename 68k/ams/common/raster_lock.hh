/*
	raster_lock.hh
	--------------
*/

#ifndef RASTERLOCK_HH
#define RASTERLOCK_HH

// iota
#include "iota/class.hh"


class raster_lock
{
	NON_COPYABLE( raster_lock )
	NO_NEW_DELETE
	
	private:
		const bool cursor_locked;
		const bool screen_locked;
	
	public:
		raster_lock( bool cursor = true, bool screen = true );
		
		~raster_lock();
};

#endif
