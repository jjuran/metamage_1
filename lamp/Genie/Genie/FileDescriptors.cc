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
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	int LowestUnusedFileDescriptor( int fd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
		return fd;
	}
	
	static FileDescriptorMap::iterator FindFDIterator( int fd )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		FileDescriptorMap::iterator it = files.find( fd );
		
		if ( it == files.end() )
		{
			p7::throw_errno( EBADF );
		}
		
		return it;
	}
	
	void CloseFileDescriptor( int fd )
	{
		FileDescriptorMap::iterator it = FindFDIterator( fd );
		
		CurrentProcess().FileDescriptors().erase( it );
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
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		(files[ fd ] = handle).closeOnExec = close_on_exec;
	}
	
	FileDescriptor& GetFileDescriptor( int fd )
	{
		FileDescriptorMap::iterator it = FindFDIterator( fd );
		
		return it->second;
	}
	
	boost::shared_ptr< IOHandle > const& GetFileHandle( int fd )
	{
		return GetFileDescriptor( fd ).handle;
	}
	
}

