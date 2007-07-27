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
	
	static int Spawn()
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
	
	static int SpawnVFork()
	{
		SystemCallFrame frame( "SpawnVFork" );
		
		return Spawn();
	}
	
	REGISTER_SYSTEM_CALL( SpawnVFork );
	
	static int fork_and_exit( int exit_status )
	{
		SystemCallFrame frame( "fork_and_exit" );
		
		if ( Spawn() == -1 )
		{
			return -1;
		}
		
		CurrentProcess().UsurpParent( exit_status );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( fork_and_exit );
	
}

