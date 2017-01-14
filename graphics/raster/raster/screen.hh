/*
	screen.hh
	---------
*/

#ifndef RASTER_SCREEN_HH
#define RASTER_SCREEN_HH

// raster
#include "raster/mb32.hh"


namespace raster
{
	
	struct raster_load;
	
	const uint32_t kStartupScreenFileType = mb32( 'S', 'C', 'R', 'N' );
	
	const int mac_screen_size = 21888;  // 512 * 342 / 8
	
	raster_load mac_screen_raster_load( void* addr );
	
}

#endif
