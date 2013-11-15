/*	==================
 *	FileDescriptors.cc
 *	==================
 */

#include "Genie/FileDescriptors.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"


namespace Genie
{
	
	int DuplicateFileDescriptor( int oldfd, int newfd, bool close_on_exec )
	{
		// Throws EBADF
		vfs::filehandle& handle = relix::get_fd_handle( oldfd );
		
		if ( oldfd != newfd )
		{
			relix::assign_fd( newfd, handle, close_on_exec );
		}
		
		return newfd;
	}
	
}

