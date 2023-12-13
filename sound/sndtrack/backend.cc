/*
	backend.cc
	----------
*/

#include "backend.hh"

// POSIX
#include <unistd.h>


namespace backend
{

static int pipe_fds[ 2 ];

bool start()
{
	return pipe( pipe_fds ) == 0;
}

bool wait()
{
	char dummy;
	return read( pipe_fds[ 0 ], &dummy, 1 ) > 0;
}

bool stop()
{
	char dummy;
	read( pipe_fds[ 0 ], &dummy, 1 );
	
	return 0;
}

void finished()
{
	close( pipe_fds[ 1 ] );
}

void interrupted()
{
	write( pipe_fds[ 1 ], "", 1 );
}

}
