/*	=========
 *	signal.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "signal.h"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/signal/caught_signal.hh"
#include "relix/syscall/getpid.hh"
#include "relix/syscall/sigpending.hh"
#include "relix/syscall/sigprocmask.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/ProcessList.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static void send_signal( Process& process, int signo )
	{
		if ( signo != 0 )
		{
			process.Raise( signo );
		}
	}
	
	static int kill_pid( pid_t pid, int signo )
	{
		if ( Process* process = FindProcess( pid ) )
		{
			send_signal( *process, signo );
			
			return 0;
		}
		
		return relix::set_errno( ESRCH );
	}
	
	struct kill_param
	{
		pid_t  id;
		int    signo;
		bool   killed_any;
	};
	
	static void* kill_process_in_group( void* param, pid_t pid, Process& process )
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
	
	static int kill_pgid( pid_t pgid, int signo )
	{
		kill_param param = { pgid ? pgid : -relix::getpid(), signo, false };
		
		for_each_process( &kill_process_in_group, &param );
		
		return param.killed_any ? 0 : relix::set_errno( ESRCH );
		
	}
	
	
	static int kill( pid_t pid, int signo )
	{
		if ( pid == 1  &&  signo == 0 )
		{
			// Optimize for canonical 'yield' idiom.
			return 0;
		}
		
		if ( signo < 0  ||  signo >= NSIG )
		{
			return set_errno( EINVAL );
		}
		
		Process& current = current_process();
		
		try
		{
			int result = pid >   0 ? kill_pid ( pid,               signo )
			           : pid ==  0 ? kill_pgid( current.GetPGID(), signo )
			           : pid == -1 ? kill_pgid( 0,                 signo )
			           :             kill_pgid( -pid,              signo );
			
			return result;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	
	static int sigaction( int signo, const struct sigaction* action, struct sigaction* oldaction )
	{
		if ( signo <= 0  ||  signo >= NSIG )
		{
			return set_errno( EINVAL );
		}
		
		Process& current = current_process();
		
		if ( oldaction != NULL )
		{
			*oldaction = current.GetSignalAction( signo );
		}
		
		if ( action != NULL )
		{
			if ( signo == SIGKILL  ||  signo == SIGSTOP )
			{
				return set_errno( EINVAL );
			}
			
		#ifdef SA_SIGINFO
			
			if ( action->sa_flags & SA_SIGINFO )
			{
				return set_errno( ENOTSUP );
			}
			
		#endif
			
			current.SetSignalAction( signo, *action );
		}
		
		return 0;
	}
	
	
	using relix::sigpending;
	using relix::sigprocmask;
	
	
	static int sigsuspend( const sigset_t* sigmask )
	{
		Process& current = current_process();
		
		sigset_t previous = current.signals_blocked();
		
		if ( sigmask != NULL )
		{
			current.set_signals_blocked( *sigmask );
		}
		
		try
		{
			while ( true )
			{
				current.Stop();
				
				current.HandlePendingSignals( true );
			}
		}
		catch ( ... )
		{
			(void) set_errno_from_exception();
		}
		
		current.set_signals_blocked( previous );
		
		relix::prevent_syscall_restart();
		
		return -1;  // EINTR
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( kill        );
	REGISTER_SYSTEM_CALL( sigaction   );
	REGISTER_SYSTEM_CALL( sigpending  );
	REGISTER_SYSTEM_CALL( sigprocmask );
	REGISTER_SYSTEM_CALL( sigsuspend  );
	
	#pragma force_active reset
	
}

