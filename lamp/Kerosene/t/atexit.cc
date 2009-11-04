/*
	atexit.cc
	---------
	
	Copyrght 2009, Joshua Juran
*/

// POSIX
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// iota
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


#define ATEXIT_MESSAGE  "at exit\n"

static const unsigned n_tests = 2;


using tap::ok_if;


static bool write_at_exit = false;

static void at_exit()
{
	if ( write_at_exit )
	{
		CHECK( write( STDOUT_FILENO, STR_LEN( ATEXIT_MESSAGE ) ) );
	}
}

static void run( const char* argv0, bool want )
{
	const char* flag = want ? "--exit" : "--_exit";
	
	int pipe_ends[2];
	
	CHECK( pipe( pipe_ends ) );
	
	pid_t pid = CHECK( vfork() );
	
	if ( pid == 0 )
	{
		CHECK( dup2( pipe_ends[1], STDOUT_FILENO ) );
		
		CHECK( close( pipe_ends[0] ) );
		CHECK( close( pipe_ends[1] ) );
		
		CHECK( execl( argv0, argv0, flag, NULL ) );
	}
	
	CHECK( close( pipe_ends[1] ) );
	
	int wait_status = -1;
	
	CHECK( wait( &wait_status ) );
	
	char buffer[ 16 ];
	
	const ssize_t n_read = CHECK( read( pipe_ends[0], buffer, sizeof buffer ) );
	
	ok_if( n_read == want * STRLEN( ATEXIT_MESSAGE ) );
	
	CHECK( close( pipe_ends[0] ) );
}

int main( int argc, const char *const *argv )
{
	if ( argc > 1 )
	{
		write_at_exit = true;
		
		CHECK( atexit( &at_exit ) );
		
		if ( strcmp( argv[1], "--_exit" ) == 0 )
		{
			_exit( 0 );
		}
		
		if ( strcmp( argv[1], "--exit" ) == 0 )
		{
			exit( 0 );
		}
		
		more::perror( "atexit-test: Invalid option", argv[1], 0 );
		
		_exit( 1 );
	}
	
	tap::start( "atexit", n_tests );
	
	run( argv[0], false );
	run( argv[0], true  );
	
	return 0;
}

