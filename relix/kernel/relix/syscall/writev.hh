/*
	writev.hh
	---------
*/

#ifndef RELIX_SYSCALL_WRITEV_HH
#define RELIX_SYSCALL_WRITEV_HH

// POSIX
#include <sys/types.h>

//#include <sys/uio.h>
struct iovec;


namespace relix
{
	
	ssize_t writev( int fd, const struct iovec* iov, int n_iov );
	
}

#endif

