/*	=================
 *	SignalReceiver.cc
 *	=================
 */

#include "Genie/Process/SignalReceiver.hh"

// Standard C++
#include <algorithm>
#include <functional>

// Standard C/C++
#include <cstring>

// Standard C
#include "errno.h"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	SignalReceiver::SignalReceiver() : itsPendingSignals(),
	                                   itsBlockedSignals()
	{
		std::memset( itsActions, 0, sizeof itsActions );
	}
	
	SignalReceiver::SignalReceiver( const SignalReceiver& other )
	: TimeKeeper(),  // Reset resource utilization on fork
	  itsPendingSignals(),  // Reset pending signals on fork
	  itsBlockedSignals( other.itsBlockedSignals )
	{
		std::memcpy( itsActions, other.itsActions, sizeof itsActions );
	}
	
	static void ResetSignalHandler( struct sigaction& action )
	{
		if ( action.sa_handler != SIG_IGN )
		{
			action.sa_handler = SIG_DFL;
		}
	}
	
	void SignalReceiver::ResetSignalHandlers()
	{
		std::for_each( itsActions,
		               itsActions + NSIG,
		               std::ptr_fun( ResetSignalHandler ) );
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
	
	bool SignalReceiver::DeliverPendingSignals( Interruptibility interrupting )
	{
		bool signal_delivered = false;
		bool return_eintr = false;
		
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
				
				itsPendingSignals &= ~signo_mask;
				
				itsBlockedSignals |= signal_mask;
				
				// (a) Account for time spent in signal handler as user time
				// (b) System time is accrued in the event of [sig]longjmp()
				LeaveSystemCall();
				
				handler( signo );
				
				EnterSystemCall( "*SIGNAL HANDLED*" );
				
				itsBlockedSignals &= ~signal_mask;
				
				signal_delivered = true;
				
				/*
					kInterruptUnlessRestarting == 1
					
					interrupting   interrupting - kInterruptUnlessRestarting
					------------   -----------------------------------------
					0 (never)      -1
					1 (unless)     0
					2 (always)     1
					
					interrupting                restartable   relation   interrupt
					------------                -----------   --------   ---------
					kInterruptAlways            x             x <= 1     true
					kInterruptUnlessRestarting  false         0 <= 0     true
					kInterruptUnlessRestarting  true          1 <= 0     false
					kInterruptNever,            x             x <= -1    false
				*/
				
				if ( !!(action.sa_flags & SA_RESTART)  <=  interrupting - kInterruptUnlessRestarting )
				{
					return_eintr = true;
				}
				
				if ( action.sa_flags & SA_RESETHAND  &&  signo != SIGILL  &&  signo != SIGTRAP )
				{
					ResetSignalAction( signo );
				}
			}
		}
		
		if ( return_eintr )
		{
			p7::throw_errno( EINTR );
		}
		
		return signal_delivered;
	}
	
}

