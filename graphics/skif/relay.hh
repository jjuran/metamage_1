/*
	relay.hh
	--------
*/

#ifndef RELAY_HH
#define RELAY_HH


namespace raster
{
	
	struct raster_load;
	
	void init_relay( const raster_load& raster );
	void stop_relay( const raster_load& raster );
	void cast_relay( const raster_load& raster );
	void bump_relay( const raster_load& raster );
	
}

#endif
