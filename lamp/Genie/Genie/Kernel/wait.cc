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
	
	namespace p7 = poseven;
	
	
	static bool StoppedWhileTracing( const Process& process )
	{
		return process.IsBeingTraced()  &&  process.GetSchedule() == kProcessStopped;
	}
	
	static Process* CheckAny( pid_t ppid, pid_t pid )
	{
		pid_t pgid = pid == -1 ? 0
		           : pid ==  0 ? CurrentProcess().GetPGID()
		           :             -pid;
		
		bool hasAnyChildren = false;
		
		typedef GenieProcessTable::iterator iterator;
		
		// FIXME:  Replace with find_if
		for ( iterator it = gProcessTable.begin();  it != gProcessTable.end();  ++it )
		{
			Process& proc = *it->second;
			
			bool is_child     =                proc.GetPPID() == ppid;
			bool pgid_matches = pgid == 0  ||  proc.GetPGID() == pgid;
			
			bool terminated   = proc.GetLifeStage() == kProcessZombie;
			
			bool traced       = StoppedWhileTracing( proc );
			
			if ( is_child && pgid_matches )
			{
				if ( terminated || traced )
				{
					return &proc;
				}
				
				hasAnyChildren = true;
			}
		}
		
		if ( !hasAnyChildren )
		{
			p7::throw_errno( ECHILD );
		}
		
		return NULL;
	}
	
	static Process* CheckPID( pid_t ppid, pid_t pid )
	{
		Process* process = FindProcess( pid );
		
		if ( process == NULL )
		{
			// No such process
			p7::throw_errno( ECHILD );
		}
		
		if ( process->GetPPID() != ppid )
		{
			// Process exists but its not your child
			p7::throw_errno( EINVAL );
		}
		
		bool terminated = process->GetLifeStage() == kProcessZombie;
		
		bool traced     = StoppedWhileTracing( *process );
		
		if ( terminated || traced )
		{
			return process;
		}
		
		// Your child is still alive, please wait...
		return NULL;
	}
	
	static pid_t waitpid( pid_t pid, int* stat_loc, int options )
	{
		SystemCallFrame frame( "waitpid" );
		
		Process& caller = frame.Caller();
		
		pid_t ppid = caller.GetPID();
		
		try
		{
			while ( true )
			{
				if ( Process* child = pid == -1 ? CheckAny( ppid, pid ) : CheckPID( ppid, pid ) )
				{
					if ( stat_loc != NULL )
					{
						*stat_loc = child->Result();
					}
					
					pid_t found_pid = child->GetPID();
					
					if ( child->GetLifeStage() == kProcessZombie )
					{
						caller.AccumulateChildTimes( child->GetTimes() );
						
						child->Release();
					}
					
					return found_pid;
				}
				
				if ( options & 1 )
				{
					// no hang
					return 0;
				}
				
				caller.Yield();
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		// Not reached
		return -1;
	}
	
	REGISTER_SYSTEM_CALL( waitpid );
	
}

