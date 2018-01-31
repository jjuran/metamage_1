/*
	signals.cc
	----------
*/

#include "Genie/api/signals.hh"

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
			self.set_pending_signal( SIGALRM );
		}
		
		bool signal_was_caught = false;
		
		const sigset_t pending = self.signals_pending();
		
		sigset_t active_signals = pending & ~self.signals_blocked();
		
		for ( int signo = 1;  signo < NSIG;  ++signo )
		{
			if ( !active_signals )
			{
				return false;
			}
			
			const sigset_t signo_mask = 1 << (signo - 1);
			
			if ( active_signals & signo_mask )
			{
				sigdelset( &active_signals, signo );
				
				const struct sigaction& action = proc.get_sigaction( signo );
				
				if ( action.sa_handler == SIG_IGN )
				{
					self.clear_pending_signal( signo );
					
					continue;
				}
				
				relix::signal signal = { signo, action };
				
				if ( action.sa_handler == SIG_DFL )
				{
					using namespace relix;
					
					self.clear_pending_signal( signo );
					
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
					const struct sigaction default_sigaction = { SIG_DFL };
					
					proc.set_sigaction( signo, default_sigaction );
				}
				
				throw signal;
			}
		}
		
		return signal_was_caught;
	}
	
}
