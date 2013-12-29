/*
	fchmod.hh
	---------
*/

#ifndef RELIX_SYSCALL_FCHMOD_HH
#define RELIX_SYSCALL_FCHMOD_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int fchmod( int fd, mode_t mode );
	
}

#endif

