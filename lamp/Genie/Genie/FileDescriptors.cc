/*	==================
 *	FileDescriptors.cc
 *	==================
 */

#include "Genie/FileDescriptors.hh"

// POSIX
#include "errno.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FileDescriptor.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	int LowestUnusedFileDescriptor( int fd )
	{
		relix::fd_table& files = CurrentProcess().FileDescriptors();
		
		return files.first_unused( fd );
	}
	
	void CloseFileDescriptor( int fd )
	{
		relix::fd_table& files = CurrentProcess().FileDescriptors();
		
		return files.close( fd );
	}
	
	int DuplicateFileDescriptor( int oldfd, int newfd, bool close_on_exec )
	{
		// Throws EBADF
		IOPtr const& handle = GetFileHandle( oldfd );
		
		if ( oldfd != newfd )
		{
			assign_file_descriptor( newfd, *handle, close_on_exec );
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
		
		relix::fd_table& files = CurrentProcess().FileDescriptors();
		
		(files[ fd ] = vfs::filehandle_ptr( &handle )).set_to_close_on_exec( close_on_exec );
	}
	
	vfs::file_descriptor& GetFileDescriptor( int fd )
	{
		relix::fd_table& files = CurrentProcess().FileDescriptors();
		
		return files.at( fd );
	}
	
	IOPtr const& GetFileHandle( int fd )
	{
		return GetFileDescriptor( fd ).handle;
	}
	
}

