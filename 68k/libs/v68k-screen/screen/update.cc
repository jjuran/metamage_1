/*
	update.cc
	---------
*/

#include "screen/update.hh"

// Standard C
#include <string.h>

// raster
#include "raster/relay.hh"
#include "raster/relay_detail.hh"

// v68k-update
#include "update/notify.hh"

// v68k-screen
#include "screen/storage.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k   {
namespace screen {

using raster::sync_relay;


sync_relay* the_sync_relay;


#ifdef __RELIX__

void update_bits()
{
}

#else

struct end_sync
{
	~end_sync()
	{
		if ( the_sync_relay )
		{
			terminate( *the_sync_relay );
		}
	}
};

static end_sync finally_end_sync;

void update_bits()
{
	void* previous_frame = virtual_buffer;
	
	if ( previous_frame )
	{
		if ( memcmp( previous_frame, transit_buffer, the_screen_size ) == 0 )
		{
			return;
		}
		
		memcpy( transit_buffer, virtual_buffer, the_screen_size );
	}
	
	if ( the_sync_relay != 0 )  // NULL
	{
		++the_sync_relay->seed;
		
		v68k::update::notify();
	}
}

#endif

}  // namespace screen
}  // namespace v68k
