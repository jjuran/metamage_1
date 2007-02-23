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
	
	int LowestUnusedFileDescriptor( int fd = 0 );
	
	void CloseFileDescriptor( int fd );
	
	void AssignFileDescriptor( int fd, const boost::shared_ptr< IOHandle >& handle );
	
	boost::shared_ptr< IOHandle > const& GetFileHandle( int fd );
	
	template < class Handle >
	inline Handle& GetCastIOHandle( int fd )
	{
		return IOHandle_Cast< Handle >( *GetFileHandle( fd ) );
	}
	
}

#endif

