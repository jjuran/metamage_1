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
	
	struct sync_relay
	{
		uint16_t  reserved;
		uint16_t  seed;
		
		pthread_mutex_t  mutex;
		pthread_cond_t   cond;
	};
	
}

#endif
