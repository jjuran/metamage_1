/*	===============
 *	ProcessGroup.cc
 *	===============
 */

#include "Genie/ProcessGroup.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace relix
{
	
	session::session()
	{
	}
	
	session::session( int id ) : its_id( id )
	{
	}
	
	session::~session()
	{
	}
	
	void session::set_ctty( vfs::filehandle& terminal )
	{
		its_terminal = &terminal;
	}
	
}

namespace Genie
{
	
	ProcessGroup::~ProcessGroup()
	{
		if ( IOHandle* handle = itsSession->get_ctty().get() )
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

