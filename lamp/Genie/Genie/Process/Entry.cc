/*	========
 *	Entry.cc
 *	========
 */

#include "Genie/Process/Entry.hh"

// Debug
#include "debug/assert.hh"


namespace Genie
{
	
	void SetUpToolScratch( Dispatcher dispatcher, iota::envp_t envp )
	{
		ASSERT( dispatcher != NULL );
		
		// Pass kernel dispatcher in ToolScratch to initialize library dispatcher
		// Pass envp in ToolScratch + 4 to initialize environ
		GetToolScratchGlobals().dispatcher = dispatcher;
		GetToolScratchGlobals().envp       = envp;
	}
	
}

