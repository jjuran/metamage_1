/*	==================
 *	FileDescriptors.hh
 *	==================
 */

#ifndef GENIE_FILEDESCRIPTORS_HH
#define GENIE_FILEDESCRIPTORS_HH

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	struct FileDescriptor;
	
	int LowestUnusedFileDescriptor( int fd = 0 );
	
	void CloseFileDescriptor( int fd );
	
	int DuplicateFileDescriptor( int oldfd, int newfd = LowestUnusedFileDescriptor() );
	
	void AssignFileDescriptor( int                                   fd,
	                           const boost::shared_ptr< IOHandle >&  handle,
	                           bool                                  close_on_exec = false );
	
	FileDescriptor& GetFileDescriptor( int fd );
	
	boost::shared_ptr< IOHandle > const& GetFileHandle( int fd );
	
	template < class Handle >
	inline Handle& GetFileHandleWithCast( int fd )
	{
		return IOHandle_Cast< Handle >( *GetFileHandle( fd ) );
	}
	
}

#endif

