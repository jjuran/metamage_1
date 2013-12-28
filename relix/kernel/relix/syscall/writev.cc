/*
	writev.cc
	---------
*/

#include "relix/syscall/writev.hh"

// POSIX
#include <sys/uio.h>

// Standard C
#include <errno.h>

// more-libc
#include "more/string.h"

// plus
#include "plus/string.hh"

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
		
		plus::string buffer;
		
		try
		{
			char* p = buffer.reset( n_bytes );
			
			for ( int i = 0;  i < n_iov;  ++i )
			{
				p = (char*) mempcpy( p, iov[ i ].iov_base, iov[ i ].iov_len );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return write( fd, buffer.data(), n_bytes );
	}
	
}

