/*
	getpgrp.cc
	----------
*/

#include "vfs/filehandle/primitives/getpgrp.hh"

// vfs
#include "vfs/filehandle.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace vfs
{
	
	pid_t getpgrp( filehandle& that )
	{
		using namespace Genie;
		
		that.terminal_methods();  // throws if not a terminal
		
		return static_cast< TerminalHandle& >( that ).getpgrp();
	}
	
}

