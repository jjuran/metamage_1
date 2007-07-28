/*	========
 *	Spawn.cc
 *	========
 */

// POSIX
#include "unistd.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_Spawn )
	DEFINE_MODULE_INIT( Kernel_Spawn )
	
	static int Spawn( SystemCallFrame& frame )
	{
		try
		{
			Process& parent = frame.Caller();
			
			Process* child = new Process( parent );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	static int SpawnVFork()
	{
		SystemCallFrame frame( "SpawnVFork" );
		
		return Spawn( frame );
	}
	
	REGISTER_SYSTEM_CALL( SpawnVFork );
	
	static int fork_and_exit( int exit_status )
	{
		SystemCallFrame frame( "fork_and_exit" );
		
		if ( Spawn( frame ) == -1 )
		{
			return -1;
		}
		
		frame.Caller().UsurpParent( exit_status );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( fork_and_exit );
	
}

