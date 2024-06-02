/*
	wait.cc
	-------
*/

// Standard C
#include <errno.h>

// POSIX
#include "sys/wait.h"

// poseven
#include "poseven/types/errno_t.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/api/try_again.hh"
#include "relix/syscall/registry.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessList.hh"


#ifndef __RELIX__
#define __WTHREAD  0
#endif


namespace Genie
{

namespace p7 = poseven;


struct wait_param
{
	pid_t  ppid;
	pid_t  pgid;
	bool   match_untraced;
	bool   has_children;
};

static inline
pid_t get_pgid( const relix::process& proc )
{
	return proc.get_process_group().id();
}

static
void* check_process( void* param, pid_t pid, Process& process )
{
	wait_param& pb = *(wait_param*) param;
	
	if ( process.gettid() != process.GetPID() )
	{
		return NULL;  // ignore non-leader threads
	}
	
	relix::process& proc = process.get_process();
	
	const bool is_child     =                   proc.getppid()   == pb.ppid;
	const bool pgid_matches = pb.pgid == 0  ||  get_pgid( proc ) == pb.pgid;
	
	const bool terminated   = proc.is_zombie();
	const bool stopped      = proc.is_stopped();
	const bool traced       = false;
	
	if ( is_child && pgid_matches )
	{
		if ( terminated  ||  (stopped && (traced || pb.match_untraced)) )
		{
			return &process;
		}
		
		pb.has_children = true;
	}
	
	return NULL;
}

static
Process* CheckAny( pid_t ppid, pid_t pid, bool match_untraced )
{
	pid_t pgid = pid == -1 ? 0
	           : pid ==  0 ? CurrentProcess().GetPGID()
	           :             -pid;
	
	wait_param param = { ppid, pgid, match_untraced, false };
	
	if ( Process* process = (Process*) for_each_process( &check_process, &param ) )
	{
		return process;
	}
	
	if ( !param.has_children )
	{
		p7::throw_errno( ECHILD );
	}
	
	return NULL;
}

static
Process* CheckPID( pid_t ppid, pid_t pid, bool match_untraced )
{
	Process* process = FindProcess( pid );
	
	if ( process == NULL )
	{
		// No such process
		p7::throw_errno( ECHILD );
	}
	
	relix::process& proc = process->get_process();
	
	if ( proc.getppid() != ppid )
	{
		// Process exists but its not your child
		p7::throw_errno( EINVAL );
	}
	
	bool terminated = proc.is_zombie();
	bool stopped    = proc.is_stopped();
	bool traced     = false;
	
	if ( terminated  ||  (stopped && (traced || match_untraced)) )
	{
		return process;
	}
	
	// Your child is still alive, please wait...
	return NULL;
}

static
Process* CheckTID( pid_t caller, pid_t tid )
{
	Process* thread = FindProcess( tid );
	
	if ( thread == NULL )
	{
		// No such thread
		p7::throw_errno( ECHILD );
	}
	
	if ( thread->GetPID() != caller )
	{
		// Thread exists but it's not in your thread group
		p7::throw_errno( EINVAL );
	}
	
	const bool terminated = thread->GetLifeStage() == kProcessZombie;
	
	if ( terminated )
	{
		return thread;
	}
	
	// The thread is still alive, please wait...
	return NULL;
}

static
pid_t waitpid( pid_t pid, int* stat_loc, int options )
{
	relix::process& caller = relix::current_process();
	
	const pid_t ppid = caller.id();
	
	bool untraced = options & WUNTRACED;
	
	const bool is_thread = options & __WTHREAD;
	
	try
	{
		while ( true )
		{
			if ( Process* child = pid == -1 ? CheckAny( ppid, pid, untraced )
			                    : is_thread ? CheckTID( ppid, pid )
			                                : CheckPID( ppid, pid, untraced ) )
			{
				relix::process& proc = child->get_process();
				
				if ( stat_loc != NULL )
				{
					*stat_loc = proc.is_stopped() ? 0x7f : proc.status();
				}
				
				pid_t found_pid = child->GetPID();
				
				if ( child->GetLifeStage() == kProcessZombie )
				{
					if ( !is_thread )
					{
						caller.accumulate_child_times( proc.get_times() );
					}
					
					child->Release();
				}
				
				return found_pid;
			}
			
			if ( options & WNOHANG )
			{
				// no hang
				return 0;
			}
			
			relix::try_again( false );
		}
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	// Not reached
	return -1;
}

#pragma force_active on

REGISTER_SYSTEM_CALL( waitpid );

#pragma force_active reset

}
