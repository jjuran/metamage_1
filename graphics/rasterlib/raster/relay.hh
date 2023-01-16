/*
	relay.hh
	--------
*/

#ifndef RASTER_RELAY_HH
#define RASTER_RELAY_HH


namespace raster
{
	
	struct sync_relay;
	
	void publish  ( sync_relay& relay );
	void unpublish( sync_relay& relay );
	
	void broadcast( sync_relay& relay );
	void terminate( sync_relay& relay );
	
}

#endif
