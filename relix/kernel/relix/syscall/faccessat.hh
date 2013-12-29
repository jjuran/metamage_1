/*
	faccessat.hh
	------------
*/

#ifndef RELIX_SYSCALL_FACCESSAT_HH
#define RELIX_SYSCALL_FACCESSAT_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int faccessat( int dirfd, const char* path, mode_t mode, int flags );
	
}

#endif

