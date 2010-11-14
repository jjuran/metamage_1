/*	=========
 *	ptrace.cc
 *	=========
 */

// POSIX
#include "errno.h"
#include "signal.h"
#include "sys/types.h"
#include "unistd.h"

// Conventional
#include "sys/ptrace.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static int ptrace( int request, pid_t pid, void* addr, int data )
	{
	#ifdef __LAMP__
		
		Process& current = current_process();
		
		try
		{
			if ( request == PTRACE_TRACEME )
			{
				if ( current.IsBeingTraced() )
				{
					return set_errno( EPERM );
				}
				
				current.StartTracing( current.GetPPID() );
				
				return 0;
			}
			
			// Remaining requests specify target by pid
			
			if ( pid == 1 )
			{
				// Can't trace init
				return set_errno( EPERM );
			}
			
			Process& target = GetProcess( pid );
			
			if ( request == PTRACE_ATTACH )
			{
				if ( target.IsBeingTraced() )
				{
					return set_errno( EPERM );
				}
				
				target.StartTracing( current.GetPID() );
				
				return 0;
			}
			
			// Remaining requests require that the caller is tracing the target
			
			if ( target.GetTracingProcess() != current.GetPID() )
			{
				return set_errno( ESRCH );
			}
			
			if ( request == PTRACE_KILL )
			{
				target.DeliverSignal( SIGKILL );
				
				return 0;
			}
			
			// Remaining requests require tha the target be stoped
			
			if ( target.GetSchedule() != kProcessStopped )
			{
				return set_errno( ESRCH );
			}
			
			switch ( request )
			{
				case PTRACE_GETREGS:
					if ( addr == NULL )
					{
						return set_errno( EFAULT );
					}
					
					return set_errno( ENOSYS );
					
					break;
				
				case PTRACE_CONT:
					if ( data > 0  &&  data < NSIG  &&  data != SIGSTOP )
					{
						target.DeliverSignal( data );
					}
					else if ( data == 0 )
					{
						target.Continue();
					}
					else
					{
						return set_errno( EIO );
					}
					
					break;
				
				case PTRACE_DETACH:
					target.StopTracing();
					
					break;
				
				default:
					return set_errno( EINVAL );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
	#endif
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( ptrace );
	
	#pragma force_active reset
	
}

