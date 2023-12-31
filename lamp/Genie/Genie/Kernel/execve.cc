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
#include <stdlib.h>
#include <string.h>

// POSIX
#include "unistd.h"

// more-libc
#include "more/string.h"

// gear
#include "gear/inscribe_decimal.hh"

// relix
#include "relix/fs/console.hh"
#include "relix/syscall/registry.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


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
			enum
			{
				submessage_max_len = sizeof "execve: : OSStatus -1234567890\n"
			};
			
			const long buffer_size = submessage_max_len + strlen( path );
			
			char* message = (char*) alloca( buffer_size );
			
			char* q = message;
			
			q = (char*) mempcpy( q, STR_LEN( "execve: " ) );
			
			q = (char*) mempcpy( q, path, strlen( path ) );
			
			q = (char*) mempcpy( q, STR_LEN( ": OSStatus " ) );
			
			const char* errnum = gear::inscribe_decimal( err );
			
			q = (char*) mempcpy( q, errnum, strlen( errnum ) );
			
			q = (char*) mempcpy( q, STR_LEN( "\n" ) );
			
			relix::console::log( message, q - message );
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
