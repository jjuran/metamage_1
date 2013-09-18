/*
	setpgrp.cc
	----------
*/

#include "vfs/filehandle/primitives/setpgrp.hh"

// vfs
#include "vfs/filehandle.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace vfs
{
	
	void setpgrp( filehandle& that, pid_t pgid )
	{
		using namespace Genie;
		
		that.terminal_methods();  // throws if not a terminal
		
		static_cast< TerminalHandle& >( that ).setpgrp( pgid );
	}
	
}

