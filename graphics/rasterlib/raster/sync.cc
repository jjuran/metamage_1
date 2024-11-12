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
	
	const sync_relay* find_sync( const raster_note* note )
	{
		note = find( note, Note_sync );
		
		if ( note  &&  is_valid_sync( note ) )
		{
			return (const sync_relay*) data( note );
		}
		
		return NULL;
	}
	
}
