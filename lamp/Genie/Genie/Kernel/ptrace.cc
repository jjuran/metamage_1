/*	=========
 *	ptrace.cc
 *	=========
 */

// POSIX
#include "signal.h"
#include "sys/ptrace.h"
#include "unistd.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_ptrace )
	DEFINE_MODULE_INIT(  Kernel_ptrace )
	
	
	static int ptrace( int request, pid_t pid, void* addr, int data )
	{
		SystemCallFrame frame( "ptrace" );
		
		Process& current = frame.Caller();
		
		if ( pid == 1 )
		{
			return current.SetErrno( EPERM );
		}
		
		try
		{
			switch ( request )
			{
				case PTRACE_TRACEME:
					if ( current.IsBeingTraced() )
					{
						return current.SetErrno( EPERM );
					}
					
					current.StartTracing( current.GetPPID() );
					
					return 0;
				
				default:
					break;
			}
			
			Process& target = GetProcess( pid );
			
			if ( target.GetTracingProcess() != current.GetPID() )
			{
				return current.SetErrno( ESRCH );
			}
			
			switch ( request )
			{
				case PTRACE_CONT:
					
					if ( data > 0  &&  data < NSIG  &&  data != SIGSTOP )
					{
						target.DeliverSignal( data );
					}
					else
					{
						target.Continue();
					}
					
					return 0;
				
				default:
					return current.SetErrno( EINVAL );
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( ptrace );
	
}

