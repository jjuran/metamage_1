/*	=========
 *	signal.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "signal.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


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
	
	static int kill_pgid( pid_t pgid, int signo )
	{
		bool killed_any = false;
		
		Process& current = CurrentProcess();
		
		typedef GenieProcessTable::iterator iterator;
		
		for ( iterator it = GetProcessList().begin();  it != GetProcessList().end();  ++it )
		{
			Process& proc = *it->second;
			
			if ( proc.GetLifeStage() == kProcessReleased )
			{
				continue;
			}
			
			bool pgid_matches = pgid == 0 ? &proc          != &current
			                              : proc.GetPGID() == pgid;
			
			if ( pgid_matches )
			{
				send_signal( proc, signo );
				
				killed_any = true;
			}
		}
		
		return killed_any ? 0 : current.SetErrno( ESRCH );
		
	}
	
	
	static int kill( pid_t pid, int signo )
	{
		SystemCallFrame frame( "kill" );
		
		if ( pid == 1  &&  signo == 0 )
		{
			// Optimize for canonical 'yield' idiom.
			return 0;
		}
		
		if ( signo < 0  ||  signo >= NSIG )
		{
			return frame.SetErrno( EINVAL );
		}
		
		Process& current = frame.Caller();
		
		try
		{
			int result = pid >   0 ? kill_pid ( pid,               signo )
			           : pid ==  0 ? kill_pgid( current.GetPGID(), signo )
			           : pid == -1 ? kill_pgid( 0,                 signo )
			           :             kill_pgid( -pid,              signo );
			
			// In case we signalled ourself
			current.HandlePendingSignals( kInterruptNever );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static __sig_handler signal( int signo, __sig_handler func )
	{
		SystemCallFrame frame( "signal" );
		
		if ( signo <= 0  ||  signo >= NSIG )
		{
			frame.SetErrno( EINVAL );
			
			return SIG_ERR;
		}
		
		if ( signo == SIGKILL  ||  signo == SIGSTOP  ||  func == SIG_ERR )
		{
			frame.SetErrno( EINVAL );
			
			return SIG_ERR;
		}
		
		Process& current = frame.Caller();
		
		__sig_handler result = current.GetSignalAction( signo ).sa_handler;
		
		struct sigaction action = { func, 0, 0 };
		
		current.SetSignalAction( signo, action );
		
		return result;
	}
	
	
	static int sigaction( int signo, const struct sigaction* action, struct sigaction* oldaction )
	{
		SystemCallFrame frame( "sigaction" );
		
		if ( signo <= 0  ||  signo >= NSIG )
		{
			return frame.SetErrno( EINVAL );
		}
		
		Process& current = frame.Caller();
		
		if ( oldaction != NULL )
		{
			*oldaction = current.GetSignalAction( signo );
		}
		
		if ( action != NULL )
		{
			if ( signo == SIGKILL  ||  signo == SIGSTOP )
			{
				return frame.SetErrno( EINVAL );
			}
			
		#ifdef SA_SIGINFO
			
			if ( action->sa_flags & SA_SIGINFO )
			{
				return frame.SetErrno( ENOTSUP );
			}
			
		#endif
			
			current.SetSignalAction( signo, *action );
		}
		
		return 0;
	}
	
	
	static int sigpending( sigset_t* oldset )
	{
		SystemCallFrame frame( "sigpending" );
		
		if ( oldset != NULL )
		{
			*oldset = frame.Caller().GetBlockedSignals();
		}
		
		return 0;
	}
	
	
	static int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		SystemCallFrame frame( "sigprocmask" );
		
		Process& current = frame.Caller();
		
		if ( oldset != NULL )
		{
			*oldset = current.GetBlockedSignals();
		}
		
		if ( set != NULL )
		{
			switch ( how )
			{
				case SIG_SETMASK:
					current.SetBlockedSignals( *set );
					break;
				
				case SIG_BLOCK:
					current.BlockSignals( *set );
					break;
				
				case SIG_UNBLOCK:
					current.UnblockSignals( *set );
					break;
				
				default:
					return frame.SetErrno( EINVAL );
			}
		}
		
		return 0;
	}
	
	
	static int sigsuspend( const sigset_t* sigmask )
	{
		SystemCallFrame frame( "sigsuspend" );
		
		Process& current = frame.Caller();
		
		sigset_t previous = current.GetBlockedSignals();
		
		if ( sigmask != NULL )
		{
			current.SetBlockedSignals( *sigmask );
		}
		
		current.Raise( SIGSTOP );
		
		current.SetBlockedSignals( previous );
		
		return 0;
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( kill        );
	REGISTER_SYSTEM_CALL( signal      );
	REGISTER_SYSTEM_CALL( sigaction   );
	REGISTER_SYSTEM_CALL( sigpending  );
	REGISTER_SYSTEM_CALL( sigprocmask );
	REGISTER_SYSTEM_CALL( sigsuspend  );
	
	#pragma force_active reset
	
}

