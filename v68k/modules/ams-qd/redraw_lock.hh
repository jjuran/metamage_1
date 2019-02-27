/*
	redraw_lock.hh
	--------------
*/

#ifndef REDRAWLOCK_HH
#define REDRAWLOCK_HH

// ams-common
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
		
		redraw_lock( char* baseAddr1, char* baseAddr2 )
		:
			its_raster_lock( baseAddr1 == ScrnBase  ||  baseAddr2 == ScrnBase )
		{
		}
};

#endif
