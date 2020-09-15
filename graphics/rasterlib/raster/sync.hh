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
	
	const note_type Note_sync = note_type( mb32( 's', 'y', 'n', 'c' ) );
	
	bool is_valid_sync( const raster_note* note );
	
}

#endif
