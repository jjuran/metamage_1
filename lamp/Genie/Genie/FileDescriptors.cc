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
		fd_table& files = CurrentProcess().FileDescriptors();
		
		return files.first_unused( fd );
	}
	
	void CloseFileDescriptor( int fd )
	{
		fd_table& files = CurrentProcess().FileDescriptors();
		
		return files.close( fd );
	}
	
	int DuplicateFileDescriptor( int oldfd, int newfd, bool close_on_exec )
	{
		// Throws EBADF
		boost::shared_ptr< IOHandle > const& handle = GetFileHandle( oldfd );
		
		if ( oldfd != newfd )
		{
			AssignFileDescriptor( newfd, handle, close_on_exec );
		}
		
		return newfd;
	}
	
	void AssignFileDescriptor( int                                   fd,
	                           const boost::shared_ptr< IOHandle >&  handle,
	                           bool                                  close_on_exec )
	{
		if ( fd < 0 )
		{
			p7::throw_errno( EBADF );
		}
		
		fd_table& files = CurrentProcess().FileDescriptors();
		
		(files[ fd ] = handle).closeOnExec = close_on_exec;
	}
	
	FileDescriptor& GetFileDescriptor( int fd )
	{
		fd_table& files = CurrentProcess().FileDescriptors();
		
		return files.at( fd );
	}
	
	boost::shared_ptr< IOHandle > const& GetFileHandle( int fd )
	{
		return GetFileDescriptor( fd ).handle;
	}
	
}

