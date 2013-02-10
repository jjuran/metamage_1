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
		
		IOHandle_Cast< TerminalHandle >( that ).setpgrp( pgid );
	}
	
}

