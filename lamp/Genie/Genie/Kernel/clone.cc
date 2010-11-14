/*
	clone.cc
	--------
*/

// Standard C
#include <errno.h>

// Lamp
#include "lamp/sched.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/ProcessList.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


static const int supported_clone_flags = CLONE_VM
                                       | CLONE_FS
                                       | CLONE_FILES
                                       | CLONE_SIGHAND
                                       | CLONE_PARENT
                                       | CLONE_THREAD;

int _lamp_clone( int (*f)( void* ), void* stack_base, size_t stack_size, int flags, void* arg )
{
	Genie::SystemCallFrame frame( "_lamp_clone" );
	
	if ( flags & ~supported_clone_flags )
	{
		// unsupported flag
		return frame.SetErrno( EINVAL );
	}
	
	const bool share_vm      = flags & CLONE_VM;
	const bool share_fs      = flags & CLONE_FS;
	const bool share_files   = flags & CLONE_FILES;
	const bool share_sighand = flags & CLONE_SIGHAND;
	const bool share_parent  = flags & CLONE_PARENT;
	const bool clone_thread  = flags & CLONE_THREAD;
	
	if ( share_sighand  &&  !share_vm )
	{
		return frame.SetErrno( EINVAL );
	}
	
	if ( clone_thread  &&  !share_sighand )
	{
		return frame.SetErrno( EINVAL );
	}
	
	// CLONE_NEWNS is already not supported, so no conflict with CLONE_FS
	
	if ( stack_base != NULL  ||  stack_size != 0 )
	{
		// Sorry, no custom stacks yet
		return frame.SetErrno( ENOSYS );
	}
	
	if ( !share_vm )
	{
		// Sorry, no distinct address spaces
		return frame.SetErrno( ENOSYS );
	}
	
	if ( clone_thread )
	{
		// Can't do thread groups yet
		return frame.SetErrno( ENOSYS );
	}
	
	try
	{
		using namespace Genie;
		
		Process& caller = current_process();
		
		const pid_t ppid = share_parent ? caller.GetPPID()
		                                : caller.GetPID();
		
		Process& child = *NewProcess( caller, ppid );
		
		if ( !share_fs )
		{
			child.unshare_fs_info();
		}
		
		if ( !share_files )
		{
			child.unshare_files();
		}
		
		if ( !share_sighand )
		{
			child.unshare_signal_handlers();
		}
		
		child.SpawnThread( f, arg );
		
		return child.GetPID();
	}
	catch ( ... )
	{
		return frame.SetErrnoFromException();
	}
	
	return 0;
}


#pragma force_active on

REGISTER_SYSTEM_CALL( _lamp_clone );

#pragma force_active reset

