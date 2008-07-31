/*	=================
 *	SignalReceiver.cc
 *	=================
 */

#include "Genie/Process/SignalReceiver.hh"

// Standard C/C++
#include <cstring>

// Nucleus
#include "Nucleus/NAssert.h"


namespace Genie
{
	
	SignalReceiver::SignalReceiver() : itsPendingSignals(),
	                                   itsBlockedSignals()
	{
		std::memset( itsHandlers, 0, sizeof itsHandlers );
	}
	
	__sig_handler SignalReceiver::GetSignalAction( int signo ) const
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		return itsHandlers[ signo - 1 ];
	}
	
	void SignalReceiver::SetSignalAction( int signo, __sig_handler action )
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		itsHandlers[ signo - 1 ] = action;
	}
	
	void SignalReceiver::ResetSignalAction( int signo )
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		itsHandlers[ signo - 1 ] = SIG_DFL;
	}
	
	bool SignalReceiver::DeliverPendingSignals()
	{
		sigset_t previousSignals = itsPendingSignals;
		
		for ( int signo = 1;  itsPendingSignals && signo < NSIG;  ++signo )
		{
			sigset_t signal_mask = 1 << signo - 1;
			
			if ( ~itsBlockedSignals & itsPendingSignals & signal_mask )
			{
				__sig_handler action = itsHandlers[ signo - 1 ];
				
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

