/*
	pwrite.hh
	---------
*/

#ifndef RELIX_SYSCALL_PWRITE_HH
#define RELIX_SYSCALL_PWRITE_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t pwrite( int fd, const void* buf, size_t count, off_t offset );
	
}

#endif

