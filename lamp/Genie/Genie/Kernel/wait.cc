/*	=======
 *	wait.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/wait.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static Process* CheckAny( pid_t ppid, pid_t pid, bool match_untraced )
	{
		pid_t pgid = pid == -1 ? 0
		           : pid ==  0 ? CurrentProcess().GetPGID()
		           :             -pid;
		
		bool hasAnyChildren = false;
		
		typedef GenieProcessTable::iterator iterator;
		
		// FIXME:  Replace with find_if
		for ( iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
		{
			Process& proc = *it->second;
			
			bool is_child     =                proc.GetPPID() == ppid;
			bool pgid_matches = pgid == 0  ||  proc.GetPGID() == pgid;
			
			bool terminated   = proc.GetLifeStage() == kProcessZombie;
			
			bool stopped      = proc.GetSchedule() == kProcessStopped;
			
			bool traced       = proc.IsBeingTraced();
			
			if ( is_child && pgid_matches )
			{
				if ( terminated  ||  stopped && (traced || match_untraced) )
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
	
	static Process* CheckPID( pid_t ppid, pid_t pid, bool match_untraced )
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
		
		bool stopped    = process->GetSchedule() == kProcessStopped;
		
		bool traced     = process->IsBeingTraced();
		
		if ( terminated  ||  stopped && (traced || match_untraced) )
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
		
		bool untraced = options & WUNTRACED;
		
		try
		{
			while ( true )
			{
				if ( Process* child = pid == -1 ? CheckAny( ppid, pid, untraced )
				                                : CheckPID( ppid, pid, untraced ) )
				{
					if ( stat_loc != NULL )
					{
						*stat_loc = child->GetSchedule() == kProcessStopped ? 0x7f : child->Result();
					}
					
					pid_t found_pid = child->GetPID();
					
					if ( child->GetLifeStage() == kProcessZombie )
					{
						caller.AccumulateChildTimes( child->GetTimes() );
						
						child->Release();
					}
					
					return found_pid;
				}
				
				if ( options & WNOHANG )
				{
					// no hang
					return 0;
				}
				
				// wait is restartable
				Yield( kInterruptUnlessRestarting );
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		// Not reached
		return -1;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( waitpid );
	
	#pragma force_active reset
	
}

