/*
	accept.cc
	---------
*/

#include "relix/syscall/accept.hh"

// Standard C
#include <errno.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/primitives/accept.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int accept( int listener, struct sockaddr* addr, socklen_t* addrlen )
	{
		try
		{
			if ( addrlen == NULL  &&  addr != NULL )
			{
				// If you pass the address buffer you must indicate the size
				set_errno( EINVAL );
			}
			
			sockaddr dummy_addr;
			
			socklen_t dummy_length = sizeof dummy_addr;
			
			sockaddr& address = addr != NULL ? *addr : dummy_addr;
			
			// addr != NULL  implies  addrlen != NULL
			socklen_t& length = addr != NULL ? *addrlen : dummy_length;
			
			vfs::filehandle_ptr incoming = accept( get_fd_handle( listener ), address, length );
			
			int fd = first_free_fd();
			
			assign_fd( fd, *incoming );
			
			if ( addr == NULL  &&  addrlen != NULL )
			{
				// You can pass a NULL address buffer and still get the size back
				*addrlen = length;
			}
			
			return fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

