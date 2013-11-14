/*	==================
 *	FileDescriptors.cc
 *	==================
 */

#include "Genie/FileDescriptors.hh"

// POSIX
#include "errno.h"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/file_descriptor.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/get_fds.hh"
#include "relix/task/fd_map.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	int LowestUnusedFileDescriptor( int fd )
	{
		relix::fd_map& files = relix::get_fds();
		
		return files.first_unused( fd );
	}
	
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
	
	void assign_file_descriptor( int               fd,
	                             vfs::filehandle&  handle,
	                             bool              close_on_exec )
	{
		if ( fd < 0 )
		{
			p7::throw_errno( EBADF );
		}
		
		relix::fd_map& files = relix::get_fds();
		
		(files[ fd ] = vfs::filehandle_ptr( &handle )).set_to_close_on_exec( close_on_exec );
	}
	
	vfs::file_descriptor& GetFileDescriptor( int fd )
	{
		relix::fd_map& files = relix::get_fds();
		
		return files.at( fd );
	}
	
	vfs::filehandle& get_filehandle( int fd )
	{
		return *GetFileDescriptor( fd ).handle;
	}
	
}

