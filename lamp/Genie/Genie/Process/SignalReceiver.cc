/*	=================
 *	SignalReceiver.cc
 *	=================
 */

#include "Genie/Process/SignalReceiver.hh"

// Nucleus
#include "Nucleus/NAssert.h"


namespace Genie
{
	
	SignalReceiver::SignalReceiver() : itsPendingSignals(),
	                                   itsBlockedSignals(),
	                                   itsMaskedSignals()
	{
	}
	
	__sig_handler SignalReceiver::SetSignalAction( int signal, __sig_handler action )
	{
		if ( signal == SIGKILL  ||  signal == SIGSTOP  ||  action == SIG_ERR )
		{
			//errno = EINVAL;  // FIXME
			return SIG_ERR;
		}
		
		__sig_handler& mapped_action = itsSignalMap[ signal ];
		
		__sig_handler result = mapped_action;
		
		mapped_action = action;
		
		return result;
	}
	
	bool SignalReceiver::DeliverPendingSignals()
	{
		sigset_t previousSignals = itsPendingSignals;
		
		for ( int signal = 1;  itsPendingSignals && signal < NSIG;  ++signal )
		{
			sigset_t signal_mask = 1 << signal - 1;
			
			if ( ~itsMaskedSignals & itsPendingSignals & signal_mask )
			{
				__sig_handler action = itsSignalMap[ signal ];
				
				ASSERT( action != SIG_IGN );
				ASSERT( action != SIG_DFL );
				
				itsPendingSignals &= ~signal_mask;
				
				itsMaskedSignals |= signal_mask;
				
				action( signal );
				
				itsMaskedSignals &= itsBlockedSignals | ~signal_mask;
			}
		}
		
		return previousSignals;
	}
	
}

