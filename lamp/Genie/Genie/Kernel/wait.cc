/*	=======
 *	wait.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/wait.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_wait )
	DEFINE_MODULE_INIT( Kernel_wait )
	
	namespace P7 = POSeven;
	
	static Process* CheckAnyPID( pid_t ppid )
	{
		bool hasAnyChildren = false;
		
		typedef GenieProcessTable::iterator iterator;
		
		// FIXME:  Replace with find_if
		for ( iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetPPID() == ppid )
			{
				if ( proc.GetLifeStage() == kProcessTerminated )
				{
					return &proc;
				}
				
				hasAnyChildren = true;
			}
		}
		
		if ( !hasAnyChildren )
		{
			P7::ThrowErrno( ECHILD );
		}
		
		return NULL;
	}
	
	static Process* CheckPID( pid_t ppid, pid_t pid )
	{
		Process* process = FindProcess( pid );
		
		if ( process == NULL )
		{
			// No such process
			P7::ThrowErrno( ECHILD );
		}
		
		if ( process->GetPPID() != ppid )
		{
			// Process exists but its not your child
			P7::ThrowErrno( EINVAL );
		}
		
		if ( process->GetLifeStage() != kProcessTerminated )
		{
			// Your child is still alive, please wait...
			return NULL;
		}
		
		return process;
	}
	
	static pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		pid_t ppid = CurrentProcess().GetPID();
		
		try
		{
			while ( true )
			{
				if ( Process* process = pid == -1 ? CheckAnyPID( ppid ) : CheckPID( ppid, pid ) )
				{
					if ( stat_loc != NULL )
					{
						*stat_loc = process->Result();
					}
					
					process->Release();
					
					return process->GetPID();  // Not affected by Release()
				}
				
				if ( options & 1 )
				{
					// no hang
					return 0;
				}
				
				Yield();
			}
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		// Not reached
		return -1;
	}
	
	REGISTER_SYSTEM_CALL( waitpid );
	
}

