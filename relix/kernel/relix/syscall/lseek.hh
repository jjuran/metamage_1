/*
	lseek.hh
	--------
*/

#ifndef RELIX_SYSCALL_LSEEK_HH
#define RELIX_SYSCALL_LSEEK_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	off_t lseek( int fd, off_t offset, int whence );
	
}

#endif

