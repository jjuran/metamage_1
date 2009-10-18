/*	=========
 *	execve.cc
 *	=========
 */

// Standard C
#include <stdio.h>

// POSIX
#include "unistd.h"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static int execve( const char*        path,
	                   const char* const  argv[],
	                   const char* const  envp[] )
	{
		// On a successful exec, the frame doesn't get destructed, but we compensate.
		// Exec() calls Suspend(), which is equivalent to LeaveSystemCall().
		// ResumeAfterFork() calls Resume() and LeaveSystemCall().
		SystemCallFrame frame( "execve" );
		
		try
		{
			Process& current( frame.Caller() );
			
			bool forked = current.Forked();
			
			// Start a new thread with the child's process context
			current.Exec( path, argv, envp );
			
			// If we didn't fork, we're now toast.
			
			// A non-forked exec kills its own thread and doesn't return
			ASSERT( forked );
			
			// Hope nothing bad happened while we thought we were still the child
			
			// Have the parent longjmp() back into vfork() and resume
			GetProcess( current.GetPPID() ).ResumeAfterFork();
			
			// Not reached
		}
		catch ( const N::OSStatus& err )
		{
			if ( err == resNotFound )
			{
				return frame.SetErrno( EINVAL );
			}
			
			std::string errMsg = "\n";
			
			try
			{
				throw;
			}
			catch ( const N::ErrMessage& msg )
			{
				errMsg = ", errMessage: " + NN::Convert< std::string >( msg.errMessage ) + "\n";
			}
			catch ( ... )  {}
			
			if ( err.Get() != fnfErr )
			{
				std::printf( "OSStatus %d%s", int( err.Get() ), errMsg.c_str() );
			}
			
			return frame.SetErrnoFromException();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		// Not reached
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( execve );
	
	#pragma force_active reset
	
}

