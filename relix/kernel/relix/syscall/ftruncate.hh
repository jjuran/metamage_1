/*
	ftruncate.hh
	------------
*/

#ifndef RELIX_SYSCALL_FTRUNCATE_HH
#define RELIX_SYSCALL_FTRUNCATE_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int ftruncate( int fd, off_t length );
	
}

#endif

