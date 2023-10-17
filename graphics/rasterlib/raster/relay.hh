/*
	relay.hh
	--------
*/

#ifndef RASTER_RELAY_HH
#define RASTER_RELAY_HH

// raster
#include "raster/relay_detail.hh"


namespace raster
{
	
	inline
	void broadcast( sync_relay& relay )
	{
		++relay.seed;
	}
	
	inline
	void terminate( sync_relay& relay )
	{
		relay.status = Sync_ended;
		
		++relay.seed;
	}
	
}

#endif
