/*
	load.hh
	-------
*/

#ifndef RASTER_LOAD_HH
#define RASTER_LOAD_HH

// raster
#include "raster/raster.hh"


namespace raster
{
	
	enum
	{
		RasterSync_none,
		RasterSync_playing,
		RasterSync_driving = -1,
	};
	
	raster_load open_raster( void* addr, long end, bool synchronized = false );
	raster_load open_raster( int fd, int raster_sync );
	
	inline raster_load load_raster( int fd )  { return open_raster( fd, 0 ); }
	inline raster_load play_raster( int fd )  { return open_raster( fd, 1 ); }
	inline raster_load drive_raster( int fd )  { return open_raster( fd, -1 ); }
	
	raster_load create_raster( int fd );
	
	void close_raster( raster_load& raster );
	
}

#endif
