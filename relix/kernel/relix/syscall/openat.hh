/*
	openat.hh
	---------
*/

#ifndef RELIX_SYSCALL_OPENAT_HH
#define RELIX_SYSCALL_OPENAT_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int openat( int dirfd, const char* path, int flags, mode_t mode );
	
}

#endif

