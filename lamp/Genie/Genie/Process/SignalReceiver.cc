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
	                                   itsBlockedSignals()
	{
	}
	
	__sig_handler SignalReceiver::SetSignalAction( int signo, __sig_handler action )
	{
		__sig_handler& mapped_action = itsSignalMap[ signo ];
		
		__sig_handler result = mapped_action;
		
		mapped_action = action;
		
		return result;
	}
	
	bool SignalReceiver::DeliverPendingSignals()
	{
		sigset_t previousSignals = itsPendingSignals;
		
		for ( int signo = 1;  itsPendingSignals && signo < NSIG;  ++signo )
		{
			sigset_t signal_mask = 1 << signo - 1;
			
			if ( ~itsBlockedSignals & itsPendingSignals & signal_mask )
			{
				__sig_handler action = itsSignalMap[ signo ];
				
				ASSERT( action != SIG_IGN );
				ASSERT( action != SIG_DFL );
				
				itsPendingSignals &= ~signal_mask;
				
				itsBlockedSignals |= signal_mask;
				
				action( signo );
				
				itsBlockedSignals &= ~signal_mask;
			}
		}
		
		return previousSignals;
	}
	
}

