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
		
		return IOHandle_Cast< TerminalHandle >( that ).getpgrp();
	}
	
}

