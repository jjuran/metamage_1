/*	=========
 *	ptrace.cc
 *	=========
 */

// POSIX
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

