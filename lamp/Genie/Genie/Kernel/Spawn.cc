/*	========
 *	Spawn.cc
 *	========
 */

// BSD
#include "unistd.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_Spawn )
	DEFINE_MODULE_INIT( Kernel_Spawn )
	
	static int SpawnVFork()
	{
		try
		{
			Process& parent = CurrentProcess();
			
			Process* child = new Process( parent );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( SpawnVFork );
	
}

