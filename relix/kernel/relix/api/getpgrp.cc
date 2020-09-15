/*
	getpgrp.cc
	----------
*/

#include "vfs/filehandle/primitives/getpgrp.hh"

// vfs
#include "vfs/filehandle.hh"

// relix
#include "relix/fs/terminal.hh"


namespace vfs
{
	
	pid_t getpgrp( filehandle& that )
	{
		using relix::terminal_extra;
		
		that.terminal_methods();  // throws if not a terminal
		
		terminal_extra& extra = *(terminal_extra*) that.extra();
		
		return extra.pgid;
	}
	
}
