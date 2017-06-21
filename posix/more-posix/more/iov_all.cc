/*
	more/iov_all.cc
	---------------
*/

#include "more/iov_all.hh"

// POSIX
#include <errno.h>


namespace more
{
	
	int iov_all( iov_proc f, int fd, iovec* iov, int n, cancel_test test )
	{
		iovec* it  = iov;
		iovec* end = iov + n;
		
		while ( it < end )
		{
			if ( test )
			{
				test();
			}
			
			ssize_t n_bytes = f( fd, it, n );
			
			if ( n_bytes > 0 )
			{
				while ( n_bytes >= it->iov_len )
				{
					n_bytes -= it->iov_len;
					++it;
					--n;
				}
				
				if ( n_bytes > 0 )
				{
					it->iov_base = (char*) it->iov_base + n_bytes;
					it->iov_len  =         it->iov_len  - n_bytes;
				}
			}
			else if ( n_bytes == 0 )
			{
				errno = EIO;
				return -1;
			}
			else if ( errno == EINTR )
			{
				continue;
			}
			else
			{
				return n_bytes;  // -1
			}
		}
		
		return 0;
	}
	
}

