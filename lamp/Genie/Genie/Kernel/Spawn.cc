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
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( SpawnVFork    );
	REGISTER_SYSTEM_CALL( fork_and_exit );
	
	#pragma force_active reset
	
}

