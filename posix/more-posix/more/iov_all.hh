/*
	more/iov_all.hh
	---------------
*/

#ifndef MORE_IOVALL_HH
#define MORE_IOVALL_HH

// POSIX
#include <sys/uio.h>


namespace more
{
	
	typedef ssize_t (*iov_proc)( int fd, const iovec* iov, int n );
	
	typedef void (*cancel_test)();
	
	
	int iov_all( iov_proc f, int fd, iovec* iov, int n, cancel_test test = 0 );
	
	inline
	int readv_all( int fd, iovec* iov, int n, cancel_test test = 0 )
	{
		return iov_all( &readv, fd, iov, n, test );
	}
	
	inline
	int writev_all( int fd, iovec* iov, int n, cancel_test test = 0 )
	{
		return iov_all( &writev, fd, iov, n, test );
	}
	
}

#endif
