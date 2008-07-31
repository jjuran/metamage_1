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
		std::memset( itsActions, 0, sizeof itsActions );
	}
	
	const struct sigaction& SignalReceiver::GetSignalAction( int signo ) const
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		return itsActions[ signo - 1 ];
	}
	
	void SignalReceiver::SetSignalAction( int signo, const struct sigaction& action )
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		itsActions[ signo - 1 ] = action;
	}
	
	void SignalReceiver::ResetSignalAction( int signo )
	{
		ASSERT( signo >    0 );
		ASSERT( signo < NSIG );
		
		struct sigaction default_sigaction = { SIG_DFL, 0, 0 };
		
		itsActions[ signo - 1 ] = default_sigaction;
	}
	
	bool SignalReceiver::WaitsForChildren() const
	{
		const struct sigaction& chld = itsActions[ SIGCHLD - 1 ];
		
		enum
		{
			sa_nocldwait
			
		#ifdef SA_NOCLDWAIT
			
			= SA_NOCLDWAIT
			
		#endif
			
		};
		
		return chld.sa_handler != SIG_IGN  &&  (chld.sa_flags & sa_nocldwait) == 0;
	}
	
	bool SignalReceiver::DeliverPendingSignals()
	{
		bool will_interrupt = false;
		
		for ( int signo = 1;  itsPendingSignals && signo < NSIG;  ++signo )
		{
			const struct sigaction& action = itsActions[ signo - 1 ];
			
			const sigset_t signo_mask = 1 << signo - 1;
			
			if ( ~itsBlockedSignals & itsPendingSignals & signo_mask )
			{
				sigset_t signal_mask = action.sa_mask;
				
				if ( !(action.sa_flags & (SA_NODEFER | SA_RESETHAND)) )
				{
					signal_mask |= signo_mask;
				}
				
				const __sig_handler handler = action.sa_handler;
				
				ASSERT( handler != SIG_IGN );
				ASSERT( handler != SIG_DFL );
				
				itsPendingSignals &= ~signal_mask;
				
				itsBlockedSignals |= signal_mask;
				
				handler( signo );
				
				itsBlockedSignals &= ~signal_mask;
				
				if ( !(action.sa_flags & SA_RESTART) )
				{
					will_interrupt = true;
				}
				
				if ( action.sa_flags & SA_RESETHAND  &&  signo != SIGILL  &&  signo != SIGTRAP )
				{
					ResetSignalAction( signo );
				}
			}
		}
		
		return will_interrupt;
	}
	
}

