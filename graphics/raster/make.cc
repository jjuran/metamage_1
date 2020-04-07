/*
	make.cc
	-------
*/

#include "make.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// rasterlib
#include "raster/load.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"


#define PROGRAM  "raster"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_relay    = 'R',
	Opt_count    = 'c',
	Opt_force    = 'f',
	Opt_geometry = 'g',
	Opt_model    = 'm',
};

static command::option options[] =
{
	{ "count",    Opt_count,    command::Param_required },
	{ "force",    Opt_force                             },
	{ "geometry", Opt_geometry, command::Param_required },
	{ "model",    Opt_model,    command::Param_required },
	{ "relay",    Opt_relay                             },
	{ NULL }
};

struct geometry_spec
{
	uint32_t width;
	uint32_t height;
	uint32_t weight;
};

static geometry_spec the_geometry;

static unsigned the_count = 1;

static raster::raster_model the_model = raster::Model_none;

static bool force_create  = false;
static bool include_relay = false;

static const char* raster_models[] =
{
	"paint",
	"light",
	"palette",
	"RGB",
	"xRGB",
	"RGBx",
	"ARGB",
	"RGBA",
};

static
geometry_spec parse_geometry( const char* p )
{
	using gear::parse_unsigned_decimal;
	
	uint32_t width = parse_unsigned_decimal( &p );
	
	if ( width == 0 )
	{
		WARN( "width can't be zero" );
		exit( 2 );
	}
	
	if ( *p++ == '\0' )
	{
		WARN( "height missing from geometry" );
		exit( 2 );
	}
	
	uint32_t height = parse_unsigned_decimal( &p );
	
	if ( height == 0 )
	{
		WARN( "height can't be zero" );
		exit( 2 );
	}
	
	if ( *p++ == '\0' )
	{
		WARN( "depth missing from geometry" );
		exit( 2 );
	}
	
	uint32_t weight = parse_unsigned_decimal( &p );
	
	if ( weight == 0 )
	{
		WARN( "depth can't be zero" );
		exit( 2 );
	}
	
	if ( weight > 32 )
	{
		WARN( "depth can't exceed 32" );
		exit( 2 );
	}
	
	if ( weight - 1 & weight )
	{
		WARN( "depth must be a power of two" );
		exit( 2 );
	}
	
	geometry_spec g = { width, height, weight };
	
	return g;
}

static
raster::raster_model lookup_model( const char* name )
{
	const int n = sizeof raster_models / sizeof raster_models[ 0 ];
	
	for ( int i = 0;  i < n;  ++i )
	{
		if ( strcmp( raster_models[ i ], name ) == 0 )
		{
			return raster::raster_model( i );
		}
	}
	
	WARN( "invalid model" );
	exit( 2 );
	
	// not reached
	return raster::raster_model();
}

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		using gear::parse_unsigned_decimal;
		
		switch ( opt )
		{
			case Opt_count:
				the_count = parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_force:
				force_create = true;
				break;
			
			case Opt_geometry:
				the_geometry = parse_geometry( global_result.param );
				break;
			
			case Opt_model:
				the_model = lookup_model( global_result.param );
				break;
			
			case Opt_relay:
				include_relay = true;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
uint32_t make_stride( uint32_t width, int weight )
{
	if ( weight > 8 )
	{
		return width * (weight / 8);
	}
	
	if ( weight < 8 )
	{
		int pixels_per_byte = 8 / weight;
		
		width += pixels_per_byte - 1;
		width /= pixels_per_byte;
	}
	
	// width is now in bytes
	
	const int alignment = 2;
	const int mask = alignment - 1;
	
	return width + mask & ~mask;
}

static
uint32_t sizeof_raster( uint32_t raster_size )
{
	using namespace raster;
	
	const uint32_t minimum_footer_size = sizeof (raster_metadata)
	                                   + sizeof (raster_note) * include_relay
	                                   + sizeof (sync_relay)  * include_relay
	                                   + sizeof (uint32_t);
	
	const uint32_t disk_block_size = 512;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (raster_size + minimum_footer_size + k) & ~k;
	
	return total_size;
}

static inline
uint32_t sizeof_footer( uint32_t raster_size )
{
	return sizeof_raster( raster_size ) - raster_size;
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
int create_raster_file( const char* path, const geometry_spec& geometry )
{
	using namespace raster;
	
	int flags = force_create ? O_RDWR | O_CREAT
	                         : O_RDWR | O_CREAT | O_EXCL;
	
	int fd = open( path, flags, 0664 );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	const uint32_t width  = geometry.width;
	const uint32_t height = geometry.height;
	const uint32_t weight = geometry.weight;
	
	const uint32_t stride     = make_stride( width, weight );
	const uint32_t image_size = height * stride;
	const uint32_t raster_size = image_size * the_count;
	
	int nok = ftruncate( fd, sizeof_raster( raster_size ) );
	
	if ( nok < 0 )
	{
		return errno;
	}
	
	raster_load raster = create_raster( fd );
	
	close_without_errno( fd );
	
	if ( raster.addr == 0 )  // NULL
	{
		return errno;
	}
	
	const uint32_t footer_size = sizeof_footer( raster_size );
	
	memset( raster.addr, '\xFF', raster.size );
	
	raster.meta = (raster_metadata*) ((char*) raster.addr + raster_size);
	
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
	desc.model  = the_model;
	desc.extra  = the_count - 1;
	
	raster_note* next_note = &note;
	
	if ( include_relay )
	{
		next_note->type = Note_sync;
		next_note->size = sizeof (sync_relay);
		
		next_note = next( next_note );
	}
	
	next_note->type = Note_end;
	
	return 0;
}

int make_raster( char** argv )
{
	char* const* args = get_options( argv - 1 );
	
	const char* path = *args++;
	
	if ( path == NULL )
	{
		WARN( "path argument required" );
		return 2;
	}
	
	if ( the_geometry.width == 0 )
	{
		WARN( "`--geometry <width>x<height>*<depth>` is required" );
		return 2;
	}
	
	if ( unsigned( the_count - 1 ) > 255 )
	{
		WARN( "`--count` must be between 1 and 256 inclusive" );
		return 2;
	}
	
	if ( the_model < 0 )
	{
		// Pick a sensible default pixel model based on the pixel weight.
		
		switch ( the_geometry.weight )
		{
			using namespace raster;
			
			default:  // should't happen
			case 1:
			case 2:
			case 4:
			case 8:
				the_model = Model_grayscale_paint;  // 0 = white, 1 = black
				break;
			
			case 16:
			#if ! defined( __MACOS__ )  &&  ! defined( __APPLE__ )
				the_model = Model_RGB;  // 5/6/5
				break;
			#else
			//	the_model = Model_xRGB;  // 1/5/5/5
			//	break;
			#endif
			
			case 32:
				the_model = Model_xRGB;  // 8/8/8/8
				break;
		}
	}
	
	int err = create_raster_file( path, the_geometry );
	
	if ( err )
	{
		perror( path );
		return 1;
	}
	
	return 0;
}
