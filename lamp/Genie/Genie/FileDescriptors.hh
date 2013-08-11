/*	==================
 *	FileDescriptors.hh
 *	==================
 */

#ifndef GENIE_FILEDESCRIPTORS_HH
#define GENIE_FILEDESCRIPTORS_HH

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/IO/IOHandle_Cast.hh"


namespace vfs
{
	
	struct file_descriptor;
	
}

namespace Genie
{
	
	int LowestUnusedFileDescriptor( int fd = 0 );
	
	void CloseFileDescriptor( int fd );
	
	int DuplicateFileDescriptor( int   oldfd,
	                             int   newfd,
	                             bool  close_on_exec = false );
	
	void assign_file_descriptor( int               fd,
	                             vfs::filehandle&  handle,
	                             bool              close_on_exec = false );
	
	vfs::file_descriptor& GetFileDescriptor( int fd );
	
	IOPtr const& GetFileHandle( int fd );
	
	template < class Handle >
	inline Handle& GetFileHandleWithCast( int fd, int errnum = 0 )
	{
		return IOHandle_Cast< Handle >( *GetFileHandle( fd ), errnum );
	}
	
}

#endif

