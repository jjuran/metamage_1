/*	================
 *	fork_and_exit.hh
 *	================
 */

#ifndef FORKANDEXIT_HH
#define FORKANDEXIT_HH

// POSIX
#include <unistd.h>

#ifdef __GNUC__

inline int fork_and_exit( int status )
{
	int forked = fork();
	
	if ( forked > 0 )
	{
		_exit( status );
	}
	
	return forked;  // 0 for child or -1 on error
}

#endif

#endif

