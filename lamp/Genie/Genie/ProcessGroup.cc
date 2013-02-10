/*	===============
 *	ProcessGroup.cc
 *	===============
 */

#include "Genie/ProcessGroup.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	ProcessGroup::~ProcessGroup()
	{
		if ( IOHandle* handle = itsSession->get_ctty().get() )
		{
			TerminalHandle& terminal = IOHandle_Cast< TerminalHandle >( *handle );
			
			if ( terminal.getpgrp() == id() )
			{
				terminal.setpgrp( no_pgid );
			}
		}
	}
	
	boost::intrusive_ptr< Session > NewSession( pid_t sid )
	{
		return new Session( sid );
	}
	
}

