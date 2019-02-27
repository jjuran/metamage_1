/*
	redraw_lock.hh
	--------------
*/

#ifndef REDRAWLOCK_HH
#define REDRAWLOCK_HH

// ams-common
#include "raster_lock.hh"


struct BitMap;


class redraw_lock
{
	private:
		raster_lock its_raster_lock;
	
	public:
		redraw_lock( const BitMap& bitmap );
		
		redraw_lock( const BitMap& oneBits, const BitMap& twoBits );
};

#endif
