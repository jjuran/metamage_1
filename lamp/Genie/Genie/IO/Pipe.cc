/*	=======
 *	Pipe.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"


namespace Genie
{
	
	PipeInHandle::~PipeInHandle()
	{
		itsConduit->close_ingress();
	}
	
	
	PipeOutHandle::~PipeOutHandle()
	{
		itsConduit->close_egress();
	}
	
}

