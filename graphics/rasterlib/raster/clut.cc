/*
	clut.cc
	-------
*/

#include "raster/clut.hh"

// raster
#include "raster/clut_detail.hh"


namespace raster
{

bool is_valid_clut( const raster_note* note )
{
	if ( note == NULL  ||  note->size < sizeof (clut_header) )
	{
		return false;
	}
	
	const clut_header& clut = data< clut_header >( *note );
	
	return clut.max > 0  &&  note->size >= sizeof_clut( clut );
}

}
