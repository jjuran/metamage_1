/*
	bytemap.cc
	----------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// jack
#include "jack/fifo.hh"
#include "jack/interface.hh"

// raster
#include "raster/load.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"


#define PROGRAM  "bytemap"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )

using raster::raster_load;
using raster::sync_relay;


enum
{
	Opt_file = 'f',
	
	Opt_width  = 'w',
	Opt_height = 'h',
	
	Opt_interval = 'i',
};

static command::option options[] =
{
	{ "file", Opt_file, command::Param_required },
	{ "width", Opt_width, command::Param_required },
	{ "height", Opt_height, command::Param_required },
	{ "interval", Opt_interval, command::Param_required },
	{ NULL }
};

static uint32_t width  = 256;
static uint32_t height = 192;

const uint8_t weight = 8;

static const char* graphics_file;

static int interval;

static void* the_screen_buffer;

static sync_relay* the_sync_relay;


static
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static inline
uint32_t sizeof_raster( uint32_t image_size )
{
	using namespace raster;
	
	const uint32_t minimum_footer_size = sizeof (raster_metadata)
	                                   + sizeof (raster_note)
	                                   + sizeof (sync_relay)
	                                   + sizeof (uint32_t);
	
	const uint32_t disk_block_size = 512;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (image_size + minimum_footer_size + k) & ~k;
	
	return total_size;
}

static inline
uint32_t sizeof_footer( uint32_t image_size )
{
	return sizeof_raster( image_size ) - image_size;
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
sync_relay& initialize( raster_load& raster, uint32_t width, uint32_t height )
{
	using namespace raster;
	
	const uint32_t stride     = width / 8 * weight;
	const uint32_t image_size = height * stride;
	
	const uint32_t footer_size = sizeof_footer( image_size );
	
	memset( raster.addr, '\xFF', raster.size );
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + image_size);
	
	uint32_t* end = (uint32_t*) ((char*) raster.meta + footer_size);
	
	*--end = footer_size;
	
	raster_metadata& meta = *raster.meta;
	
	raster_desc& desc = meta.desc;
	raster_note& note = meta.note;
	
	memset( &meta, '\0', sizeof meta );
	
	desc.width  = width;
	desc.height = height;
	desc.stride = stride;
	desc.weight = weight;
	
	note.type = Note_sync;
	note.size = sizeof (sync_relay);
	
	raster_note& last = next( note );
	
	last.type = Note_end;
	
	return data< sync_relay >( note );
}

static
int create_raster( const char* path, uint32_t width, uint32_t height )
{
	using namespace raster;
	
	int fd = open( path, O_RDWR | O_CREAT, 0664 );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	const uint32_t stride     = width / 8 * weight;
	const uint32_t image_size = height * stride;
	
	int nok = ftruncate( fd, sizeof_raster( image_size ) );
	
	if ( nok < 0 )
	{
		return errno;
	}
	
	raster_load raster = create_raster( fd );
	
	the_screen_buffer = raster.addr;
	
	close_without_errno( fd );
	
	if ( the_screen_buffer == 0 )  // NULL
	{
		return errno;
	}
	
	sync_relay& sync = initialize( raster, width, height );
	
	publish( sync );
	
	the_sync_relay = &sync;
	
	jack::interface ji = path;
	
	const char* fifo_path = ji.fifo_path();
	
	int fifo_fd = jack::fifo_ready( fifo_path, 0622 );
	
	if ( fifo_fd < 0 )
	{
		return errno;
	}
	
	return 0;
}

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Opt_file:
				graphics_file = global_result.param;
				break;
			
			case Opt_width:
				width = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_height:
				height = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_interval:
				interval = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;  // whatever
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( graphics_file == NULL )
	{
		ERROR( "-f argument required" );
		return 2;
	}
	
	if ( width == 0  ||  height == 0 )
	{
		ERROR( "width and height must be non-zero" );
		return 2;
	}
	
	int err = create_raster( graphics_file, width, height );
	
	if ( err )
	{
		report_error( graphics_file, err );
		return 1;
	}
	
	if ( interval < 10 )
	{
		interval = 100;
	}
	
	interval *= 1000;
	
	const uint32_t stride     = width / 8 * weight;
	const uint32_t image_size = height * stride;
	
	uint8_t byte = 0;
	
	while ( true )
	{
		memset( the_screen_buffer, byte++, image_size );
		
		raster::broadcast( *the_sync_relay );
		
		usleep( interval );
	}
	
	return 0;
}
