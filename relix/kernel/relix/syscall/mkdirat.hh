/*
	mkdirat.hh
	----------
*/

#ifndef RELIX_SYSCALL_MKDIRAT_HH
#define RELIX_SYSCALL_MKDIRAT_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int mkdirat( int dirfd, const char* path, mode_t mode );
	
}

#endif

