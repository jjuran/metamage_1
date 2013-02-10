/*
	process_group.cc
	----------------
*/

#include "relix/task/process_group.hh"

// vfs
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/primitives/setpgrp.hh"


namespace relix
{
	
	process_group::~process_group()
	{
		if ( vfs::filehandle* handle = its_session->get_ctty().get() )
		{
			// FIXME:  Assert that handle is a terminal
			
			if ( getpgrp( *handle ) == id() )
			{
				setpgrp( *handle, no_pgid );
			}
		}
	}
	
}

