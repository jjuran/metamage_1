/*
	call_signal_handler.cc
	----------------------
*/

#include "relix/signal/call_signal_handler.hh"

// relix
#include "relix/api/current_thread.hh"
#include "relix/signal/signal.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	void call_signal_handler( const signal& sig )
	{
		sigset_t signal_mask = sig.action.sa_mask;
		
		if ( !(sig.action.sa_flags & (SA_NODEFER | SA_RESETHAND)) )
		{
			sigaddset( &signal_mask, sig.number );
		}
		
		thread& self = current_thread();
		
		self.clear_pending_signal( sig.number );
		
		const sigset_t blocked_signals = self.signals_blocked();
		
		self.block_signals( signal_mask );
		
		sig.action.sa_handler( sig.number );
		
		self.set_signals_blocked( blocked_signals );
	}
	
}
