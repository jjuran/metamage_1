/*
	synch.cc
	--------
*/

#include "synch.hh"

// POSIX
#include <unistd.h>


namespace synch
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

void finished()
{
	close( pipe_fds[ 1 ] );
}

void interrupted()
{
	write( pipe_fds[ 1 ], "", 1 );
}

}
