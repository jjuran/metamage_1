/*
	duplex.cc
	---------
*/

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <signal.h>
#include <stdlib.h>

// more-posix
#include "more/perror.hh"

// gear
#include "gear/parse_decimal.hh"


#define PROGRAM  "duplex"

#define USAGE "Usage: duplex <in-fd>-<out-fd> ...\n" \
              " e.g.  duplex 0-7 6-1\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#ifdef ANDROID
#define FORK() fork()
#else
#define FORK() vfork()
#endif


static const char* cat = "/bin/cat";


static
int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	return 2;
}

static
bool spawn( const char* stream )
{
	const int a = gear::parse_unsigned_decimal( &stream );
	
	if ( *stream++ != '-' )
	{
		write( STDERR_FILENO, STR_LEN( "duplex: malformed stream spec\n" ) );
		return false;
	}
	
	const int b = gear::parse_unsigned_decimal( &stream );
	
	if ( *stream++ != '\0' )
	{
		write( STDERR_FILENO, STR_LEN( "duplex: malformed stream spec\n" ) );
		return false;
	}
	
	if ( a == STDOUT_FILENO  ||  a == STDERR_FILENO )
	{
		write( STDERR_FILENO, STR_LEN( "duplex: invalid read from 1 or 2\n" ) );
		return false;
	}
	
	if ( b == STDIN_FILENO  ||  b == STDERR_FILENO )
	{
		write( STDERR_FILENO, STR_LEN( "duplex: invalid write to 0 or 2\n" ) );
		return false;
	}
	
	pid_t pid = FORK();
	
	if ( pid < 0 )
	{
		more::perror( PROGRAM );
		return false;
	}
	
	if ( pid == 0 )
	{
		if ( dup2( a, STDIN_FILENO  ) < 0 )  goto fail;
		if ( dup2( b, STDOUT_FILENO ) < 0 )  goto fail;
		
		execl( cat, "cat", NULL );
		
	fail:
		
		const int exit_code = errno == ENOENT ? 127 : 126;
		
		more::perror( PROGRAM, cat );
		
		_exit( exit_code );
	}
	
	return true;
}

int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		return usage();
	}
	
	// Form a new process group so we don't kill the parent process.
	
	if ( setpgid( 0, 0 ) < 0 )
	{
		more::perror( PROGRAM, "setpgid" );
		return 1;
	}
	
	int failed = 0;
	
	for ( int i = 1;  i < argc;  ++i )
	{
		const char* stream = argv[ i ];
		
		/*
			spawn() both parses arguments and launches subprocesses.
			In the future, we may wish to parse in advance in order to detect
			any errors before taking partial action.  Additionally, if a use
			case ever requires that a spawned child process have the last
			remaining reference to a filehandle (e.g. so that close() will end
			the process), then it will be necessary to make three passes:  One
			to parse and set the close-on-exec flag, a second to spawn child
			processes, and a third to close the specified file descriptors.
		*/
		
		failed = ! spawn( stream );
		
		if ( failed )
		{
			goto done;
		}
	}
	
	// Wait for any subprocess to terminate, then kill the others and exit.
	
	int wait_status;
	
	wait( &wait_status );
	
done:
	
	signal( SIGHUP, SIG_IGN );
	
	kill( 0, SIGHUP );
	
	return failed;
}
