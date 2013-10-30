/*
	call_signal_handler.cc
	----------------------
*/

#include "relix/signal/call_signal_handler.hh"

// relix
#include "relix/api/current_thread.hh"
#include "relix/signal/caught_signal.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	void call_signal_handler( const caught_signal& signal )
	{
		sigset_t signal_mask = signal.action.sa_mask;
		
		if ( !(signal.action.sa_flags & (SA_NODEFER | SA_RESETHAND)) )
		{
			signal_mask |= sigset_from_signo( signal.signo );
		}
		
		thread& self = current_thread();
		
		self.clear_pending_signal( signal.signo );
		
		const sigset_t blocked_signals = self.signals_blocked();
		
		self.block_signals( signal_mask );
		
		signal.action.sa_handler( signal.signo );
		
		self.set_signals_blocked( blocked_signals );
	}
	
}

