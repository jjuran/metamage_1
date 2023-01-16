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
#include "raster/relay_detail.hh"
#include "raster/sync.hh"


#define PROGRAM  "skif"

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

void stop_relay( const raster_load& raster )
{
	terminate( get_relay( raster ) );
}

void cast_relay( const raster_load& raster )
{
	broadcast( get_relay( raster ) );
}

void bump_relay( const raster_load& raster )
{
	sync_relay& relay = get_relay( raster );
	
	++relay.seed;
	
	broadcast( relay );
}

}  // namespace raster
