/*
	telecast-recv.cc
	----------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/cond.hh"
#include "poseven/types/mutex.hh"

// telekine
#include "telekine/commands.hh"

// telecast-send
#include "raster.hh"


#define PROGRAM  "telecast-recv"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


namespace p7 = poseven;

using namespace telekine;  // FIXME

static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
plus::string make_path()
{
	const char* home = getenv( "HOME" );
	
	if ( home == NULL )
	{
		home = "";
	}
	
	plus::var_string path = home;
	
	path += "/var/run/raster/recv-";
	
	path += gear::inscribe_unsigned_decimal( getpid() );
	
	return path.move();
}

enum eof_disposition
{
	EOF_fatal,
	EOF_okay,
};

static
bool read_POD( int fd, void* buffer, size_t n, eof_disposition eof )
{
	ssize_t n_read = read( fd, buffer, n );
	
	if ( n_read < 0 )
	{
		report_error( "recv", errno );
		exit( 1 );
	}
	
	if ( n_read == 0  &&  eof != EOF_fatal )
	{
		return false;
	}
	
	if ( n_read != n )
	{
		report_error( "recv", EIO );
		exit( 1 );
	}
	
	return true;
}

template < class Structure >
static inline
void read_structure( int fd, Structure& structure )
{
	read_POD( fd, &structure, sizeof structure, EOF_fatal );
	
	convert_big_endian( structure );
}

template < class Structure >
static inline
bool read_structure_or_eof( int fd, Structure& structure )
{
	if ( read_POD( fd, &structure, sizeof structure, EOF_okay ) )
	{
		convert_big_endian( structure );
		
		return true;
	}
	
	return false;
}

static
pid_t spawn( const char* path, char** argv )
{
	if ( *argv == NULL )
	{
		return -1;
	}
	
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		report_error( "fork", pid );
		exit( 1 );
	}
	
	if ( pid == 0 )
	{
		char** arg = argv;
		
		while ( *++arg != NULL )
		{
			if ( strcmp( *arg, "-" ) == 0 )
			{
				*arg = const_cast< char* >( path );
				break;
			}
		}
		
		execvp( *argv, argv );
		
		report_error( "*argv", errno );
		_exit( 1 );
	}
	
	return pid;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	const plus::string path = make_path();
	
	raster_task* task = NULL;
	
	raster_desc desc;
	
	pid_t coprocess_pid = -1;
	
	int fd = STDIN_FILENO;
	
	command_header header;
	
	while ( read_structure_or_eof( fd, header ) )
	{
		if ( header.command == Command_desc )
		{
			printf( "DESC (%d bytes)\n", header.size );
			
			if ( header.size != sizeof desc )
			{
				fprintf( stderr, "Bad desc size\n" );
				return 1;
			}
			
			if ( task )
			{
				fprintf( stderr, "Duplicate DESC\n" );
				return 1;
			}
			
			read_structure( fd, desc );
			
			fprintf( stderr, "%dx%d*%d\n", desc.width, desc.height, desc.weight );
			
			task = new raster_task( path.c_str(), desc );
			
			coprocess_pid = spawn( path.c_str(), argv + 1 );
		}
		else if ( header.command == Command_rect )
		{
			rect_pv16 rect;
			
			read_structure( fd, rect );
			
			if ( task == NULL )
			{
				fprintf( stderr, "RECT before DESC\n" );
				return 1;
			}
			
			task->recv_rect( STDIN_FILENO, rect );
		}
		else
		{
			fprintf( stderr, "Bad command\n" );
			return 1;
		}
	}
	
	int nok = unlink( path.c_str() );
	
	if ( nok )
	{
		report_error( path.c_str(), errno );
	}
	
	delete task;
	
	int wait_status;
	wait( &wait_status );
	
	return 0;
}
