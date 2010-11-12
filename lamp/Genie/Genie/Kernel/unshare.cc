/*
	unshare.cc
	----------
*/

// Standard C
#include <errno.h>

// Lamp
#include "lamp/sched.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


static const int supported_clone_flags = CLONE_FS
                                       | CLONE_FILES
                                       | CLONE_NEWNS;

int unshare( int flags )
{
	Genie::SystemCallFrame frame( "unshare" );
	
	if ( flags & ~supported_clone_flags )
	{
		// unsupported flag
		return frame.SetErrno( EINVAL );
	}
	
	const bool unshare_fs    = flags & CLONE_FS;
	const bool unshare_files = flags & CLONE_FILES;
	const bool unshare_newns = flags & CLONE_NEWNS;
	
	if ( unshare_newns )
	{
		return frame.SetErrno( EPERM );
	}
	
	try
	{
		using namespace Genie;
		
		Process& caller = frame.Caller();
		
		if ( unshare_fs )
		{
			caller.unshare_fs_info();
		}
		
		if ( unshare_files )
		{
			caller.unshare_files();
		}
	}
	catch ( ... )
	{
		return frame.SetErrnoFromException();
	}
	
	return 0;
}


#pragma force_active on

REGISTER_SYSTEM_CALL( unshare );

#pragma force_active reset

