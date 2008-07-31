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
	
	DECLARE_MODULE_INIT( Kernel_signal )
	DEFINE_MODULE_INIT(  Kernel_signal )
	
	
	static void send_signal( Process& process, int sig )
	{
		if ( sig != 0 )
		{
			process.Raise( sig );
		}
	}
	
	static int kill_pid( pid_t pid, int sig )
	{
		if ( Process* process = FindProcess( pid ) )
		{
			send_signal( *process, sig );
			
			return 0;
		}
		
		return CurrentProcess().SetErrno( ESRCH );
	}
	
	static int kill_pgid( pid_t pgid, int sig )
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
				send_signal( proc, sig );
				
				killed_any = true;
			}
		}
		
		return killed_any ? 0 : current.SetErrno( ESRCH );
		
	}
	
	static int kill( pid_t pid, int sig )
	{
		SystemCallFrame frame( "kill" );
		
		Process& current = CurrentProcess();
		
		int result = pid >   0 ? kill_pid ( pid,               sig )
		           : pid ==  0 ? kill_pgid( current.GetPGID(), sig )
		           : pid == -1 ? kill_pgid( 0,                 sig )
		           :             kill_pgid( -pid,              sig );
		
		// In case we signalled ourself
		current.HandlePendingSignals();
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( kill );
	
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
		
		return frame.Caller().SetSignalAction( signo, func );
	}
	
	REGISTER_SYSTEM_CALL( signal );
	
	static __sig_handler sigaction( int signo, const struct sigaction* action, struct sigaction* oldaction )
	{
		SystemCallFrame frame( "sigaction" );
		
		if ( signo <= 0  ||  signo >= NSIG )
		{
			frame.SetErrno( EINVAL );
			
			return SIG_ERR;
		}
		
		Process& current = frame.Caller();
		
		if ( oldaction != NULL )
		{
			oldaction->sa_mask  = 0;
			oldaction->sa_flags = 0;
			
			oldaction->sa_handler = current.GetSignalAction( signo );
		}
		
		if ( action != NULL )
		{
			if ( signo == SIGKILL  ||  signo == SIGSTOP )
			{
				frame.SetErrno( EINVAL );
				
				return SIG_ERR;
			}
			
		#ifdef SA_SIGINFO
			
			if ( action->sa_flags & SA_SIGINFO )
			{
				frame.SetErrno( ENOTSUP );
				
				return SIG_ERR;
			}
			
		#endif
			
			current.SetSignalAction( signo, action->sa_handler );
		}
		
		frame.SetErrno( ENOSYS );
		
		return SIG_ERR;
	}
	
	REGISTER_SYSTEM_CALL( sigaction );
	
	static int sigpending( sigset_t* oldset )
	{
		SystemCallFrame frame( "sigpending" );
		
		if ( oldset != NULL )
		{
			*oldset = frame.Caller().GetBlockedSignals();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( sigpending );
	
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
	
	REGISTER_SYSTEM_CALL( sigprocmask );
	
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
	
	REGISTER_SYSTEM_CALL( sigsuspend );
	
}

