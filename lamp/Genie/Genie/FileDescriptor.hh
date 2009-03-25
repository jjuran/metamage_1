/*	=================
 *	FileDescriptor.hh
 *	=================
 */

#ifndef GENIE_FILEDESCRIPTOR_HH
#define GENIE_FILEDESCRIPTOR_HH

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class IOHandle;
	
	struct FileDescriptor
	{
		boost::shared_ptr< IOHandle >  handle;
		bool                           closeOnExec;
		
		FileDescriptor() : closeOnExec( false )  {}
		
		FileDescriptor( const boost::shared_ptr< IOHandle >& handle ) : handle     ( handle ),
		                                                                closeOnExec( false  )
		{
		}
		
	};
	
}

#endif

