/*
	hangup.cc
	---------
*/

#include "vfs/filehandle/primitives/hangup.hh"

// vfs
#include "vfs/filehandle.hh"

// Genie
#include "Genie/IO/Terminal.hh"


namespace vfs
{
	
	void hangup( filehandle& that )
	{
		using namespace Genie;
		
		that.terminal_methods();  // throws if not a terminal
		
		static_cast< TerminalHandle& >( that ).Disconnect();
	}
	
}

