/*
	fork_and_exit.h
	---------------
	
	Joshua Juran
*/

#ifndef LAMP_FORKANDEXIT_H
#define LAMP_FORKANDEXIT_H

// POSIX
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

pid_t fork_and_exit( int status );

#ifdef __cplusplus
}
#endif


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

