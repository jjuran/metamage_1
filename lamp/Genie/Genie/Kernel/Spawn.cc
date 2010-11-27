/*	========
 *	Spawn.cc
 *	========
 */

// POSIX
#include "unistd.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static int vfork_start( const _vfork_pad* pad )
	{
		current_process().set_resume_handler( pad );
		
		try
		{
			Process& child = current_process().vfork();
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	static int fork_and_exit( int exit_status )
	{
		try
		{
			current_process().fork_and_exit( exit_status );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( vfork_start   );
	REGISTER_SYSTEM_CALL( fork_and_exit );
	
	#pragma force_active reset
	
}

