/*	================
 *	fork_and_exit.hh
 *	================
 */

#ifndef FORKANDEXIT_HH
#define FORKANDEXIT_HH

// POSIX
#include <unistd.h>

pid_t fork_and_exit( int status );

#ifdef __GNUC__

inline pid_t fork_and_exit( int status )
{
	pid_t forked = fork();
	
	if ( forked > 0 )
	{
		_exit( status );
	}
	
	return forked;  // 0 for child or -1 on error
}

#endif

#endif

