/*
	redraw_lock.hh
	--------------
*/

#ifndef REDRAWLOCK_HH
#define REDRAWLOCK_HH

// ams-common
#include "raster_lock.hh"


class redraw_lock
{
	private:
		raster_lock its_raster_lock;
	
	public:
		redraw_lock( char* baseAddr );
		
		redraw_lock( char* baseAddr1, char* baseAddr2 );
};

#endif
