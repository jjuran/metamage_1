/*
	pread.hh
	--------
*/

#ifndef RELIX_SYSCALL_PREAD_HH
#define RELIX_SYSCALL_PREAD_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t pread( int fd, void* buf, size_t count, off_t offset );
	
}

#endif

