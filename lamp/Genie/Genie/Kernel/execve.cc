/*	=========
 *	execve.cc
 *	=========
 */

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// Standard C
#include <errno.h>
#include <stdio.h>

// POSIX
#include "unistd.h"

// more-libc
#include "more/string.h"

// relix
#include "relix/syscall/registry.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


#ifndef __MACOS__
namespace {
#endif

int execve( char  const*  path,
            char* const*  argv,
            char* const*  envp )
{
	using namespace Genie;
	
	/*
		On a successful exec, we don't return to the dispatcher, but we
		compensate:
		
		Exec() calls Suspend(), which is equivalent to LeaveSystemCall().
		ResumeAfterFork() calls Resume() and LeaveSystemCall().
	*/
	
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
		
		int num = err;
		
		if ( num != fnfErr )
		{
			printf( "execve: %s: OSStatus %d\n", path, num );
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

#ifndef __MACOS__
}
#endif
