/*	==================
 *	FileDescriptors.cc
 *	==================
 */

#include "Genie/FileDescriptors.hh"

// vfs
#include "vfs/file_descriptor.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/get_fd.hh"
#include "relix/api/get_fds.hh"
#include "relix/task/fd_map.hh"


namespace Genie
{
	
	int DuplicateFileDescriptor( int oldfd, int newfd, bool close_on_exec )
	{
		// Throws EBADF
		vfs::filehandle& handle = get_filehandle( oldfd );
		
		if ( oldfd != newfd )
		{
			relix::assign_fd( newfd, handle, close_on_exec );
		}
		
		return newfd;
	}
	
	vfs::file_descriptor& GetFileDescriptor( int fd )
	{
		relix::fd_map& files = relix::get_fds();
		
		return files.at( fd );
	}
	
	vfs::filehandle& get_filehandle( int fd )
	{
		return *relix::get_fd( fd ).handle;
	}
	
}

