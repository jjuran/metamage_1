/*
	readlinkat.hh
	-------------
*/

#ifndef RELIX_SYSCALL_READLINKAT_HH
#define RELIX_SYSCALL_READLINKAT_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t _readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags );
	
}

#endif

