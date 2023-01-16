/*
	relay.cc
	--------
*/

#include "raster/relay.hh"

// raster
#include "raster/relay_detail.hh"


namespace raster
{
	
	void broadcast( sync_relay& relay )
	{
		++relay.seed;
	}
	
	void terminate( sync_relay& relay )
	{
		relay.status = Sync_ended;
		
		++relay.seed;
	}
	
}
