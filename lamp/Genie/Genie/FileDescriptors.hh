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
	
	int DuplicateFileDescriptor( int   oldfd,
	                             int   newfd,
	                             bool  close_on_exec = false );
	
	vfs::file_descriptor& GetFileDescriptor( int fd );
	
	vfs::filehandle& get_filehandle( int fd );
	
	template < class Handle >
	inline Handle& GetFileHandleWithCast( int fd, int errnum = 0 )
	{
		return IOHandle_Cast< Handle >( get_filehandle( fd ), errnum );
	}
	
}

#endif

