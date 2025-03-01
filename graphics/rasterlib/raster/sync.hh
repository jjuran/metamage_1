/*
	sync.hh
	-------
*/

#ifndef RASTER_SYNC_HH
#define RASTER_SYNC_HH

// raster
#include "raster/mb32.hh"
#include "raster/note.hh"


namespace raster
{
	
	struct sync_relay;
	
	const note_type Note_sync = note_type( mb32( 's', 'y', 'n', 'c' ) );
	
	bool is_valid_sync( const raster_note* note );
	
	const sync_relay* find_sync( const raster_note* note );
	
	inline
	sync_relay* find_sync( raster_note* note )
	{
		const raster_note* const_note = note;
		
		return const_cast< sync_relay* >( find_sync( const_note ) );
	}
	
}

#endif
