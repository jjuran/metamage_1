/*	=========
 *	execve.cc
 *	=========
 */

// Standard C
#include <errno.h>
#include <stdio.h>

// POSIX
#include "unistd.h"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/string/concat.hh"

// relix
#include "relix/syscall/registry.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

#include "Nitrogen/CodeFragments.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace Genie
{

namespace N = Nitrogen;


static int execve( const char*        path,
                   const char* const  argv[],
                   const char* const  envp[] )
{
	// On a successful exec, we don't return to the dispatcher, but we compensate.
	// Exec() calls Suspend(), which is equivalent to LeaveSystemCall().
	// ResumeAfterFork() calls Resume() and LeaveSystemCall().
	
	try
	{
		Process& current = current_process();
		
		// Start a new thread with the child's process context
		current.Exec( path, argv, envp );
		
		// Not reached
	}
	catch ( const Mac::OSStatus& err )
	{
		if ( err == resNotFound )
		{
			return set_errno( EINVAL );
		}
		
		plus::string errMsg = "\n";
		
		try
		{
			throw;
		}
		catch ( const N::ErrMessage& msg )
		{
			errMsg = ", errMessage: " + plus::string( msg.errMessage ) + "\n";
		}
		catch ( ... )  {}
		
		if ( err.Get() != fnfErr )
		{
			std::printf( "execve: %s: OSStatus %d%s", path, int( err.Get() ), errMsg.c_str() );
		}
		
		return set_errno_from_exception();
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	// Not reached
	return 0;
}

#pragma force_active on

REGISTER_SYSTEM_CALL( execve );

#pragma force_active reset

}
