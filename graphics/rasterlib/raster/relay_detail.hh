/*
	relay.hh
	--------
*/

#ifndef RASTER_RELAYDETAIL_HH
#define RASTER_RELAYDETAIL_HH

// POSIX
#include <pthread.h>

// Standard C
#include <stdint.h>


namespace raster
{
	
	enum sync_status
	{
		Sync_invalid = -1,
		Sync_ready   = 0,
		Sync_ended   = 1,
	};
	
	struct sync_relay
	{
		int16_t   status;
		uint16_t  seed;
		
		pthread_mutex_t  mutex;
		pthread_cond_t   cond;
	};
	
}

#endif
