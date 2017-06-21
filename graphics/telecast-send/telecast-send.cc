/*
	telecast-send.cc
	----------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// poseven
#include "poseven/types/cond.hh"
#include "poseven/types/mutex.hh"

// telecast-send
#include "raster.hh"
#include "send.hh"


#define PROGRAM  "telecast-send"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


namespace p7 = poseven;

using namespace telekine;  // FIXME


enum
{
	Opt_last_byte = 255,
	
	Opt_feed,
};

static command::option options[] =
{
	{ "feed", Opt_feed, command::Param_required },
	{ NULL }
};


static const char* feed_path;

static int feed_fd = -1;


static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
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
			case Opt_feed:
				feed_path = global_result.param;
				
				feed_fd = open( feed_path, O_RDONLY );
				
				if ( feed_fd < 0 )
				{
					report_error( feed_path, errno );
					exit( 1 );
				}
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
void event_loop( const char* path )
{
	p7::mutex mutex;
	p7::cond  cond;
	
	p7::lock k( mutex );
	
	raster_task raster( path, mutex, cond, feed_path, feed_fd );
	
	const raster::raster_desc& desc = raster.desc();
	
	send_task send( STDIN_FILENO, STDOUT_FILENO, mutex, cond );
	
	send.send_desc( desc );
	
	if ( raster.single_frame() )
	{
		raster.copy_bits();
		
		rect_pv16 bounds = { 0, 0, desc.width, desc.height };
		
		raster.send_rect( STDOUT_FILENO, bounds );
		
		pause();
		exit( 0 );
	}
	
	while ( raster.running() )
	{
		cond.wait( k );
		
		rect_pv16 rect = raster.get_rect();
		
		raster.copy_bits();
		
		raster.send_rect( STDOUT_FILENO, rect );
	}
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( *args == NULL )
	{
		return 2;
	}
	
	const char* path = *args++;
	
	event_loop( path );
	
	return 0;
}
