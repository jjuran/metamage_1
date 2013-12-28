/*
	read.hh
	-------
*/

#ifndef RELIX_SYSCALL_READ_HH
#define RELIX_SYSCALL_READ_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t read( int fd, void* buf, size_t count );
	
}

#endif

