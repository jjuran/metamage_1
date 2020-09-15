/*
	redraw_lock.hh
	--------------
*/

#ifndef REDRAWLOCK_HH
#define REDRAWLOCK_HH

// ams-common
#include "raster_lock.hh"


struct BitMap;
struct Rect;


class redraw_lock
{
	private:
		raster_lock its_raster_lock;
	
	public:
		redraw_lock( const BitMap& bitmap, const Rect& workArea );
		
		redraw_lock( const BitMap& bits1, const Rect& rect1,
		             const BitMap& bits2, const Rect& rect2 );
};

#endif
