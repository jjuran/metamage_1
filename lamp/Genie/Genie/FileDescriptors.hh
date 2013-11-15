/*	==================
 *	FileDescriptors.hh
 *	==================
 */

#ifndef GENIE_FILEDESCRIPTORS_HH
#define GENIE_FILEDESCRIPTORS_HH

// relix-kernel
#include "relix/api/get_fd_handle.hh"

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/IO/IOHandle_Cast.hh"


namespace Genie
{
	
	int DuplicateFileDescriptor( int   oldfd,
	                             int   newfd,
	                             bool  close_on_exec = false );
	
	template < class Handle >
	inline Handle& GetFileHandleWithCast( int fd, int errnum = 0 )
	{
		return IOHandle_Cast< Handle >( relix::get_fd_handle( fd ), errnum );
	}
	
}

#endif

