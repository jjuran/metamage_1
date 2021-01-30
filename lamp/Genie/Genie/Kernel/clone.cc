/*
	clone.cc
	--------
*/

// Standard C
#include <errno.h>

// relix-include
#include "relix/sched.h"

// relix-kernel
#include "relix/config/vm_fork.hh"
#include "relix/syscall/registry.hh"
#include "relix/task/process.hh"
#include "relix/task/process_resources.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessList.hh"


static const int supported_clone_flags = CLONE_VM
                                       | CLONE_FS
                                       | CLONE_FILES
                                       | CLONE_SIGHAND
                                       | CLONE_PARENT
                                       | CLONE_THREAD;

int _relix_clone( int (*f)( void* ), void* stack_base, size_t stack_size, int flags, void* arg )
{
	using namespace Genie;
	
	if ( flags & ~supported_clone_flags )
	{
		// unsupported flag
		return set_errno( EINVAL );
	}
	
	const bool share_vm      = flags & CLONE_VM;
	const bool share_fs      = flags & CLONE_FS;
	const bool share_files   = flags & CLONE_FILES;
	const bool share_sighand = flags & CLONE_SIGHAND;
	const bool share_parent  = flags & CLONE_PARENT;
	const bool clone_thread  = flags & CLONE_THREAD;
	
	if ( share_sighand  &&  !share_vm )
	{
		return set_errno( EINVAL );
	}
	
	if ( clone_thread  &&  !share_sighand )
	{
		return set_errno( EINVAL );
	}
	
	// CLONE_NEWNS is already not supported, so no conflict with CLONE_FS
	
	if ( stack_base != NULL  ||  stack_size != 0 )
	{
		// Sorry, no custom stacks yet
		return set_errno( ENOSYS );
	}
	
	if ( ! CONFIG_VM_FORK  &&  ! share_vm )
	{
		// Sorry, no distinct address spaces
		return set_errno( ENOSYS );
	}
	
	try
	{
		Process& caller = current_process();
		
		Process& child = clone_thread ? NewThread ( caller )
		                              : NewProcess( caller );
		
		relix::process&           proc = child.get_process();
		relix::process_resources& rsrcs = proc.get_process_resources();
		
		try
		{
			if ( share_parent )
			{
				proc.set_ppid( caller.GetPPID() );
			}
			
			if ( !share_fs )
			{
				rsrcs.unshare_fs_info();
			}
			
			if ( !share_files )
			{
				rsrcs.unshare_fd_map();
			}
			
			if ( !share_sighand )
			{
				proc.unshare_signal_handlers();
			}
			
			if ( CONFIG_VM_FORK  &&  ! share_vm )
			{
				proc.unshare_vm();
			}
			
			child.SpawnThread( f, arg );
		}
		catch ( ... )
		{
			relix::process& process = child.get_process();
			
			process.zombify();
			process.clear_ppid();
			
			remove_task( (relix::task*) &child );  // Bypass `protected`
			notify_reaper( &child );
			
			throw;
		}
		
		return child.gettid();
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


#pragma force_active on

REGISTER_SYSTEM_CALL( _relix_clone );

#pragma force_active reset
