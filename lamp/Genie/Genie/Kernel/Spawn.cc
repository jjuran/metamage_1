/*	========
 *	Spawn.cc
 *	========
 */

// BSD
#include "vfork.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	static int SpawnVFork()
	{
		Process* child = new Process( CurrentProcess().ProcessID() );
		
		RegisterProcessContext( child );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( SpawnVFork );
	
}

#pragma export on
	
	int SpawnVFork()
	{
		return Genie::SpawnVFork();
	}
	
#pragma export reset

