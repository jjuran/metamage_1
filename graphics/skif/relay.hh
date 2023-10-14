/*
	relay.hh
	--------
*/

#ifndef RELAY_HH
#define RELAY_HH


namespace raster
{
	
	struct raster_load;
	
	void stop_relay( const raster_load& raster );
	void bump_relay( const raster_load& raster );
	
}

#endif
