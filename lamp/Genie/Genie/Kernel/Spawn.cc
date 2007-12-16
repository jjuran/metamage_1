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
	
	inline Process& SpawnFrom( Process& parent )
	{
		Process* child = new Process( parent );
		
		return *child;
	}
	
	static int SpawnVFork( void (*LongJmp)(int), void* jmpBuf )
	{
		SystemCallFrame frame( "SpawnVFork" );
		
		frame.Caller().SetLongJmp( LongJmp );
		
		try
		{
			Process& child = SpawnFrom( frame.Caller() );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( SpawnVFork );
	
	static int fork_and_exit( int exit_status )
	{
		SystemCallFrame frame( "fork_and_exit" );
		
		try
		{
			Process& child = SpawnFrom( frame.Caller() );
			
			child.UsurpParent( exit_status );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( fork_and_exit );
	
}

