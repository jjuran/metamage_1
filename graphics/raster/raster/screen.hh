/*
	screen.hh
	---------
*/

#ifndef RASTER_SCREEN_HH
#define RASTER_SCREEN_HH


namespace raster
{
	
	struct raster_load;
	
	const int mac_screen_size = 21888;  // 512 * 342 / 8
	
	raster_load mac_screen_raster_load( void* addr );
	
}

#endif
