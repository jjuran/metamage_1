/*
	pump.hh
	-------
*/

#ifndef RELIX_SYSCALL_PUMP_HH
#define RELIX_SYSCALL_PUMP_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count, unsigned flags );
	
}

#endif
