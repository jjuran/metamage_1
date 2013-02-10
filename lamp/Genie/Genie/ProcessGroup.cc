/*	===============
 *	ProcessGroup.cc
 *	===============
 */

#include "Genie/ProcessGroup.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	Session::Session()
	{
	}
	
	Session::Session( int id ) : itsID( id )
	{
	}
	
	Session::~Session()
	{
	}
	
	void Session::SetControllingTerminal( vfs::filehandle& terminal )
	{
		itsControllingTerminal = &terminal;
	}
	
	ProcessGroup::~ProcessGroup()
	{
		if ( IOHandle* handle = itsSession->GetControllingTerminal().get() )
		{
			TerminalHandle& terminal = IOHandle_Cast< TerminalHandle >( *handle );
			
			if ( terminal.getpgrp() == ID() )
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

