/*
	select.hh
	---------
*/

#ifndef RELIX_SYSCALL_SELECT_HH
#define RELIX_SYSCALL_SELECT_HH

// POSIX
#include <sys/select.h>


namespace relix
{
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout );
	
}

#endif
