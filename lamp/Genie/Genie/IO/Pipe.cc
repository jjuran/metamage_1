/*	=======
 *	Pipe.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"


namespace Genie
{
	
	PipeInHandle::~PipeInHandle()
	{
		itsConduit->CloseIngress();
	}
	
	
	PipeOutHandle::~PipeOutHandle()
	{
		itsConduit->CloseEgress();
	}
	
}

