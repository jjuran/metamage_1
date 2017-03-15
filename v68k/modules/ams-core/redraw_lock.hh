/*
	redraw_lock.hh
	--------------
*/

#ifndef REDRAWLOCK_HH
#define REDRAWLOCK_HH

// ams-core
#include "raster_lock.hh"


char* ScrnBase : 0x0824;


class redraw_lock
{
	private:
		raster_lock its_raster_lock;
	
	public:
		redraw_lock( char* baseAddr ) : its_raster_lock( baseAddr == ScrnBase )
		{
		}
};

#endif
