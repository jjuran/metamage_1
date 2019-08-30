/*
	writev.cc
	---------
*/

#include "relix/syscall/writev.hh"

// POSIX
#include <sys/uio.h>

// Standard C
#include <errno.h>
#include <stdlib.h>

// more-libc
#include "more/string.h"

// relix
#include "relix/api/errno.hh"
#include "relix/syscall/write.hh"


namespace relix
{
	
	ssize_t writev( int fd, const struct iovec* iov, int n_iov )
	{
		size_t n_bytes = 0;
		
		bool valid = n_iov > 0;
		
		if ( valid )
		{
		#ifdef IOV_MAX
			
			valid = n_iov <= IOV_MAX;
			
		#endif
			
			if ( valid )
			{
				for ( int i = 0;  i < n_iov;  ++i )
				{
					n_bytes += iov[ i ].iov_len;
					
					if ( ssize_t( n_bytes ) < 0 )
					{
						valid = false;  // ssize_t overflow
						
						break;
					}
				}
				
			}
		}
		
		if ( !valid )
		{
			return set_errno( EINVAL );
		}
		
		if ( void* buffer = malloc( n_bytes ) )
		{
			char* p = (char*) buffer;
			
			for ( int i = 0;  i < n_iov;  ++i )
			{
				p = (char*) mempcpy( p, iov[ i ].iov_base, iov[ i ].iov_len );
			}
			
			ssize_t n_written = write( fd, buffer, n_bytes );
			
			free( buffer );
			
			return n_written;
		}
		else
		{
			return set_errno( errno );
		}
	}
	
}
