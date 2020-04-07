/*
	update.cc
	---------
*/

#include "screen/update.hh"

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/mman.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// raster
#include "raster/relay.hh"

// v68k-screen
#include "screen/storage.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


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
	static void* previous_frame;
	
	if ( previous_frame )
	{
		if ( memcmp( previous_frame, the_screen_buffer, the_screen_size ) == 0 )
		{
			return;
		}
	}
	
#ifdef __RELIX__
	
		msync( the_screen_buffer, the_screen_size, MS_SYNC );
	
#else
	
	if ( the_sync_relay != 0 )  // NULL
	{
		raster::broadcast( *the_sync_relay );
	}
	
#endif
	
	if ( previous_frame == NULL )
	{
		previous_frame = malloc( the_screen_size );
	}
	
	if ( previous_frame )
	{
		memcpy( previous_frame, the_screen_buffer, the_screen_size );
	}
}

}  // namespace screen
}  // namespace v68k
