/*
	send.hh
	-------
*/

#ifndef RELIX_SYSCALL_SEND_HH
#define RELIX_SYSCALL_SEND_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	ssize_t send( int fd, const void* buf, size_t count, int flags );
	
}

#endif
