/*
	write.hh
	--------
*/

#ifndef RELIX_SYSCALL_WRITE_HH
#define RELIX_SYSCALL_WRITE_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t write( int fd, const void* buf, size_t count );
	
}

#endif

