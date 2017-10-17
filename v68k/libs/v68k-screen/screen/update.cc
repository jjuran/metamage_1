/*
	update.cc
	---------
*/

#include "screen/update.hh"

// POSIX
#include <sys/mman.h>

// Standard C
#include <string.h>

// raster
#include "raster/relay.hh"

// v68k-screen
#include "screen/storage.hh"


#pragma exceptions off


namespace v68k   {
namespace screen {

using raster::sync_relay;


sync_relay* the_sync_relay;


struct end_sync
{
	~end_sync()
	{
		if ( the_sync_relay )
		{
			memset( the_screen_buffer, '\xFF', the_screen_size );
			
			terminate( *the_sync_relay );
		}
	}
};

#ifndef __RELIX__

static end_sync finally_end_sync;

#endif


void update()
{
#ifdef __RELIX__
	
		msync( the_screen_buffer, the_screen_size, MS_SYNC );
	
#else
	
	if ( the_sync_relay != 0 )  // NULL
	{
		raster::broadcast( *the_sync_relay );
	}
	
#endif
}

}  // namespace screen
}  // namespace v68k
