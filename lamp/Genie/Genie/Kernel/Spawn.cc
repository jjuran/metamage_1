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
	
	static inline Process& SpawnFrom( Process& parent )
	{
		const boost::shared_ptr< Process >& child = NewProcess( parent );
		
		return *child;
	}
	
	static int vfork_start( Process::long_jump_t long_jump, jmp_buf* buffer )
	{
		SystemCallFrame frame( "vfork_start" );
		
		frame.Caller().SetLongJmp( long_jump, buffer );
		
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
	
	REGISTER_SYSTEM_CALL( vfork_start   );
	REGISTER_SYSTEM_CALL( fork_and_exit );
	
	#pragma force_active reset
	
}

