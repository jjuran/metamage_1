/*
	clut.hh
	-------
*/

#ifndef RASTER_CLUT_HH
#define RASTER_CLUT_HH

// raster
#include "raster/mb32.hh"
#include "raster/note.hh"


namespace raster
{
	
	struct clut_data;
	
	const note_type Note_clut = note_type( mb32( 'c', 'l', 'u', 't' ) );
	
	bool is_valid_clut( const raster_note* note );
	
	const clut_data* find_clut( const raster_note* note );
	
}

#endif
