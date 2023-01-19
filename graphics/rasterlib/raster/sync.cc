/*
	sync.cc
	-------
*/

#include "raster/sync.hh"

// raster
#include "raster/relay_detail.hh"


namespace raster
{
	
	bool is_valid_sync( const raster_note* note )
	{
		return note != NULL  &&  note->size >= sizeof (sync_relay);
	}
	
}
