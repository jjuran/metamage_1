/*
	fchmodat.hh
	-----------
*/

#ifndef RELIX_SYSCALL_FCHMODAT_HH
#define RELIX_SYSCALL_FCHMODAT_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int fchmodat( int dirfd, const char* path, mode_t mode, int flags );
	
}

#endif

