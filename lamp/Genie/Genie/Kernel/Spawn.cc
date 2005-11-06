/*	========
 *	Spawn.cc
 *	========
 */

// BSD
#include "vfork.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


#pragma export on
	
	using Genie::CurrentProcess;
	using Genie::Process;
	
	
	int SpawnVFork()
	{
		Process* child = new Process( CurrentProcess().ProcessID() );
		
		RegisterProcessContext( child );
		
		return 0;
	}
	
#pragma export reset

