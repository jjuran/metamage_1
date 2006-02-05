/*	==========
 *	MSL_exit.c
 *	==========
 */

#ifdef __MWERKS__

#include "abort_exit.h"

// From unistd.h
extern void _exit( int status );


// Replace __atexit() so in case it's called it returns -1

int __atexit( void (*func)(void) )
{	
	// Not supported because it isn't used
	return -1;
}


// Replace __exit() to call POSIX _exit()

void __exit( int status )
{
	_exit( status );  // Doesn't return
}

#endif

