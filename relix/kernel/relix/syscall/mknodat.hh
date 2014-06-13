/*
	mknodat.hh
	----------
*/

#ifndef RELIX_SYSCALL_MKNODAT_HH
#define RELIX_SYSCALL_MKNODAT_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int mknodat( int dirfd, const char* path, mode_t mode, dev_t dev );
	
}

#endif
