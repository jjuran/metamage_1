/*
	check_signals.cc
	----------------
*/

#include "relix/signal/check_signals.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/api/thread_yield.hh"
#include "relix/signal/signal.hh"
#include "relix/signal/signal_traits.hh"
#include "relix/task/process.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	bool check_signals( bool may_throw )
	{
		thread& self = current_thread();
		
		relix::process& proc = self.get_process();
		
		if ( proc.get_alarm_clock().check() )
		{
			proc.set_pending_signal( SIGALRM );
		}
		
		bool signal_was_caught = false;
		
		sigset_t active_signals;
		
	#ifndef __linux__
		
		const sigset_t pending = self.signals_pending()
		                       | proc.signals_pending();
		
		active_signals = pending & ~self.signals_blocked();
		
	#endif
		
		for ( int signo = 1;  signo < NSIG;  ++signo )
		{
		#ifndef __linux__
			
			if ( !active_signals )
			{
				return false;
			}
			
		#endif
			
			if ( sigismember( &active_signals, signo ) )
			{
				sigdelset( &active_signals, signo );
				
				const struct sigaction& action = proc.get_sigaction( signo );
				
				if ( action.sa_handler == SIG_IGN )
				{
					self.clear_pending_signal( signo );
					proc.clear_pending_signal( signo );
					
					continue;
				}
				
				relix::signal signal = { signo, action };
				
				if ( action.sa_handler == SIG_DFL )
				{
					using namespace relix;
					
					self.clear_pending_signal( signo );
					proc.clear_pending_signal( signo );
					
					const signal_traits traits = global_signal_traits[ signo ];
					
					switch ( traits & signal_default_action_mask )
					{
						case signal_discard:
							break;
						
						case signal_terminate:
							signal.number |= traits & signal_core;
							
							throw signal;
						
						case signal_stop:
							proc.signal_stop();
							
							relix::thread_yield();
							break;
						
						case signal_continue:
							proc.signal_continue();
							break;
					}
					
					continue;
				}
				
				signal_was_caught = true;
				
				if ( !may_throw )
				{
					continue;
				}
				
				if ( action.sa_flags & SA_RESETHAND  &&  signo != SIGILL  &&  signo != SIGTRAP )
				{
					struct sigaction default_sigaction = { 0 };
					
					default_sigaction.sa_handler = SIG_DFL;
					
					proc.set_sigaction( signo, default_sigaction );
				}
				
				throw signal;
			}
		}
		
		return signal_was_caught;
	}
	
}
