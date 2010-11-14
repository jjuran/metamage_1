/*
	unshare.cc
	----------
*/

// Standard C
#include <errno.h>

// Lamp
#include "lamp/sched.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


using namespace Genie;


static const int supported_clone_flags = CLONE_FS
                                       | CLONE_FILES
                                       | CLONE_NEWNS;

int unshare( int flags )
{
	if ( flags & ~supported_clone_flags )
	{
		// unsupported flag
		return set_errno( EINVAL );
	}
	
	const bool unshare_fs    = flags & CLONE_FS;
	const bool unshare_files = flags & CLONE_FILES;
	const bool unshare_newns = flags & CLONE_NEWNS;
	
	if ( unshare_newns )
	{
		return set_errno( EPERM );
	}
	
	try
	{
		Process& caller = current_process();
		
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
		return set_errno_from_exception();
	}
	
	return 0;
}


#pragma force_active on

REGISTER_SYSTEM_CALL( unshare );

#pragma force_active reset

