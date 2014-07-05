/*
	setpgrp.cc
	----------
*/

#include "vfs/filehandle/primitives/setpgrp.hh"

// Standard C
#include <signal.h>

// vfs
#include "vfs/filehandle.hh"

// relix
#include "relix/signal/signal_process_group.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace vfs
{
	
	void setpgrp( filehandle& that, pid_t pgid )
	{
		using relix::terminal_extra;
		
		that.terminal_methods();  // throws if not a terminal
		
		terminal_extra& extra = *(terminal_extra*) that.extra();
		
		extra.pgid = pgid;
		
		if ( extra.disconnected )
		{
			relix::signal_process_group( SIGHUP, pgid );
		}
	}
	
}
