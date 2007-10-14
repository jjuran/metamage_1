/*	=======
 *	Pipe.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"

// Standard C
#include <errno.h>

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	PipeInHandle::~PipeInHandle()
	{
		itsConduit->CloseIngress();
	}
	
	int PipeInHandle::SysRead( char* data, std::size_t byteCount )
	{
		p7::throw_errno( EPERM );
		
		return -1;
	}
	
	
	PipeOutHandle::~PipeOutHandle()
	{
		itsConduit->CloseEgress();
	}
	
	int PipeOutHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		p7::throw_errno( EPERM );
		
		return -1;
	}
	
}

