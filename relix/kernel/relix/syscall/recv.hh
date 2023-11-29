/*
	recv.hh
	-------
*/

#ifndef RELIX_SYSCALL_RECV_HH
#define RELIX_SYSCALL_RECV_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t recv( int fd, void* buf, size_t count, int flags );
	
}

#endif
