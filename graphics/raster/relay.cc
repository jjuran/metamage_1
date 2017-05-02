/*
	relay.cc
	--------
*/

#include "relay.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// rasterlib
#include "raster/raster.hh"
#include "raster/relay.hh"
#include "raster/sync.hh"


#define PROGRAM  "raster"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


namespace raster
{

static
sync_relay& get_relay( const raster_load& loaded_raster )
{
	raster_note* sync = find_note( *loaded_raster.meta, Note_sync );
	
	if ( ! is_valid_sync( sync ) )
	{
		WARN( "Invalid 'sync' object in raster file" );
		exit( 3 );
	}
	
	return *(sync_relay*) data( sync );
}

void init_relay( const raster_load& raster )
{
	publish( get_relay( raster ) );
}

void cast_relay( const raster_load& raster )
{
	broadcast( get_relay( raster ) );
}

void wait_relay( const raster_load& raster )
{
	wait( get_relay( raster ) );
}

}  // namespace raster
