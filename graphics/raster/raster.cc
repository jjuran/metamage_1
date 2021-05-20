/*
	raster.cc
	---------
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

// rasterlib
#include "raster/load.hh"

// raster
#include "make.hh"
#include "relay.hh"


#define PROGRAM  "raster"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


static raster::raster_load loaded_raster;


static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
void open_raster( const char* path )
{
	int raster_fd = open( path, O_RDWR );
	
	if ( raster_fd < 0 )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	using namespace raster;
	
	loaded_raster = open_raster( raster_fd, true );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	close( raster_fd );
}

static inline
int raster_make( char** args )
{
	return make_raster( args );
}

static
int raster_info( char** args )
{
	const char* argv[] = { "skif-info", *args, NULL };
	
	execvp( *argv, (char**) argv );
	
	report_error( *argv, errno );
	
	return 1;
}

static
int raster_init( char** args )
{
	while ( const char* path = *args++ )
	{
		open_raster( path );
		
		init_relay( loaded_raster );
		
		close_raster( loaded_raster );
	}
	
	return 0;
}

static
int raster_stop( char** args )
{
	while ( const char* path = *args++ )
	{
		open_raster( path );
		
		stop_relay( loaded_raster );
		
		close_raster( loaded_raster );
	}
	
	return 0;
}

static
int raster_cast( char** args )
{
	while ( const char* path = *args++ )
	{
		open_raster( path );
		
		cast_relay( loaded_raster );
		
		close_raster( loaded_raster );
	}
	
	return 0;
}

static
int raster_bump( char** args )
{
	while ( const char* path = *args++ )
	{
		open_raster( path );
		
		bump_relay( loaded_raster );
		
		close_raster( loaded_raster );
	}
	
	return 0;
}

static
int raster_wait( char** args )
{
	while ( const char* path = *args++ )
	{
		open_raster( path );
		
		wait_relay( loaded_raster );
		
		close_raster( loaded_raster );
	}
	
	return 0;
}

static
int raster_feed( char** args )
{
	if ( const char* path = *args++ )
	{
		open_raster( path );
		
		while ( wait_relay( loaded_raster ) )
		{
			write( STDOUT_FILENO, STR_LEN( "\n" ) );
		}
		
		close_raster( loaded_raster );
	}
	
	return 0;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char** args = argv + 1;
	
	if ( *args == NULL )
	{
		return 0;
	}
	
	const char* subcommand = *args++;
	
	if ( strcmp( subcommand, "info" ) == 0 )
	{
		return raster_info( args );
	}
	
#ifndef __RELIX__
	
	if ( strcmp( subcommand, "init" ) == 0 )
	{
		return raster_init( args );
	}
	
	if ( strcmp( subcommand, "stop" ) == 0 )
	{
		return raster_stop( args );
	}
	
	if ( strcmp( subcommand, "cast" ) == 0 )
	{
		return raster_cast( args );
	}
	
	if ( strcmp( subcommand, "bump" ) == 0 )
	{
		return raster_bump( args );
	}
	
	if ( strcmp( subcommand, "wait" ) == 0 )
	{
		return raster_wait( args );
	}
	
	if ( strcmp( subcommand, "feed" ) == 0 )
	{
		return raster_feed( args );
	}
	
#endif
	
	if ( strcmp( subcommand, "make" ) == 0 )
	{
		return raster_make( args );
	}
	
	return 0;
}
