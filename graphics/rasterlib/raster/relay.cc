/*
	relay.cc
	--------
*/

#include "raster/relay.hh"

// raster
#include "raster/relay_detail.hh"


namespace raster
{
	
	void publish( sync_relay& relay )
	{
		relay.status = Sync_ready;
		relay.seed   = 0;
	}
	
	void unpublish( sync_relay& relay )
	{
	}
	
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
