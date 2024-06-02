/*
	signal.cc
	---------
*/

// Standard C
#include <errno.h>

// POSIX
#include "signal.h"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/signal/check_signals.hh"
#include "relix/syscall/getpid.hh"
#include "relix/syscall/registry.hh"
#include "relix/syscall/sigaction.hh"
#include "relix/syscall/sigpending.hh"
#include "relix/syscall/sigprocmask.hh"
#include "relix/syscall/sigsuspend.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessList.hh"


#ifndef __RELIX__
#define __SIGTHREAD  0
#endif


namespace Genie
{

static inline
void send_signal( Process& process, int signo )
{
	if ( signo != 0 )
	{
		process.Raise( signo );
	}
}

static inline
int kill_pid( pid_t pid, int signo )
{
	if ( Process* process = FindProcess( pid ) )
	{
		send_signal( *process, signo );
		
		return 0;
	}
	
	return relix::set_errno( ESRCH );
}

static inline
int kill_tid( pid_t tid, int signo )
{
	Process& current = current_process();
	
	if ( Process* target = FindProcess( tid ) )
	{
		if ( current.GetPID() == target->GetPID() )
		{
			if ( signo != 0 )
			{
				target->set_pending_signal( signo );
			}
			
			return 0;
		}
		
		/*
			Not reached by pthread_kill().
			Someone made an evil call to kill() with __SIGTHREAD set.
		*/
		
		return relix::set_errno( EINVAL );
	}
	
	return relix::set_errno( ESRCH );
}

struct kill_param
{
	pid_t  id;
	int    signo;
	bool   killed_any;
};

static
void* kill_process_in_group( void* param, pid_t pid, Process& process )
{
	if ( process.GetLifeStage() != kProcessReleased )
	{
		kill_param& pb = *(kill_param*) param;
		
		const pid_t id = pb.id;
		
		const bool matches = id < 0 ? pid != -id
		                            : process.GetPGID() == id;
		
		if ( matches )
		{
			send_signal( process, pb.signo );
			
			pb.killed_any = true;
		}
	}
	
	return NULL;
}

static
int kill_pgid( pid_t pgid, int signo )
{
	kill_param param = { pgid ? pgid : -relix::getpid(), signo, false };
	
	for_each_process( &kill_process_in_group, &param );
	
	return param.killed_any ? 0 : relix::set_errno( ESRCH );
	
}

static inline
pid_t getpgid()
{
	return current_process().GetPGID();
}

static
int kill( pid_t pid, int signo )
{
	if ( pid == 1  &&  signo == 0 )
	{
		// Optimize for canonical 'yield' idiom.
		return 0;
	}
	
	const bool is_thread = signo & __SIGTHREAD;
	
	if ( is_thread )
	{
		signo &= ~__SIGTHREAD;
	}
	
	if ( signo < 0  ||  signo >= NSIG )
	{
		return set_errno( EINVAL );
	}
	
	try
	{
		int result = is_thread ? kill_tid ( pid,       signo )
		           : pid >   0 ? kill_pid ( pid,       signo )
		           : pid ==  0 ? kill_pgid( getpgid(), signo )
		           : pid == -1 ? kill_pgid( 0,         signo )
		           :             kill_pgid( -pid,      signo );
		
		/*
			Fatal signals terminate the process immediately.
			Caught signals remain pending until reentering the kernel.
		*/
		
		relix::check_signals( false );
		
		return result;
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
}


using relix::sigaction;
using relix::sigpending;
using relix::sigprocmask;
using relix::sigsuspend;


#pragma force_active on

REGISTER_SYSTEM_CALL( kill        );
REGISTER_SYSTEM_CALL( sigaction   );
REGISTER_SYSTEM_CALL( sigpending  );
REGISTER_SYSTEM_CALL( sigprocmask );
REGISTER_SYSTEM_CALL( sigsuspend  );

#pragma force_active reset

}
