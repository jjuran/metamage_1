/*
	select.hh
	---------
*/

#ifndef RELIX_SYSCALL_SELECT_HH
#define RELIX_SYSCALL_SELECT_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/select.h>

struct timeval;


namespace relix
{
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout );
	
}

#endif
