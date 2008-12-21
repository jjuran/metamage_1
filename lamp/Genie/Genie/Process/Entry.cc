/*	========
 *	Entry.cc
 *	========
 */

#include "Genie/Process/Entry.hh"

// Nucleus
#include "Nucleus/NAssert.h"


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

