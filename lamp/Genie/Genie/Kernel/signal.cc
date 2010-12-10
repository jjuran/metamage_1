/*	=========
 *	signal.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "signal.h"

// Genie
#include "Genie/current_process.hh"
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
		
		return CurrentProcess().SetErrno( ESRCH );
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
		Process& current = CurrentProcess();
		
		kill_param param = { pgid ? pgid : -current.GetPID(), signo, false };
		
		for_each_process( &kill_process_in_group, &param );
		
		return param.killed_any ? 0 : current.SetErrno( ESRCH );
		
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
	
	
	static int sigpending( sigset_t* oldset )
	{
		if ( oldset != NULL )
		{
			*oldset = current_process().GetPendingSignals();
		}
		
		return 0;
	}
	
	
	static int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		Process& current = current_process();
		
		if ( oldset != NULL )
		{
			*oldset = current.GetBlockedSignals();
		}
		
		if ( set != NULL )
		{
			const sigset_t unblockable_mask = 1 << SIGKILL - 1 | 1 << SIGSTOP - 1;
			
			const sigset_t filtered_set = *set & ~unblockable_mask;
			
			switch ( how )
			{
				case SIG_SETMASK:
					current.SetBlockedSignals( filtered_set );
					break;
				
				case SIG_BLOCK:
					current.BlockSignals( filtered_set );
					break;
				
				case SIG_UNBLOCK:
					current.UnblockSignals( filtered_set );
					break;
				
				default:
					return set_errno( EINVAL );
			}
		}
		
		return 0;
	}
	
	
	static int sigsuspend( const sigset_t* sigmask )
	{
		Process& current = current_process();
		
		sigset_t previous = current.GetBlockedSignals();
		
		if ( sigmask != NULL )
		{
			current.SetBlockedSignals( *sigmask );
		}
		
		try
		{
			current.Stop();
			
			current.HandlePendingSignals( kInterruptAlways );
		}
		catch ( ... )
		{
			(void) set_errno_from_exception();
		}
		
		current.SetBlockedSignals( previous );
		
		return set_errno( EINTR );
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( kill        );
	REGISTER_SYSTEM_CALL( sigaction   );
	REGISTER_SYSTEM_CALL( sigpending  );
	REGISTER_SYSTEM_CALL( sigprocmask );
	REGISTER_SYSTEM_CALL( sigsuspend  );
	
	#pragma force_active reset
	
}

