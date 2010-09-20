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

// Debug
#include "debug/assert.hh"


namespace Genie
{
	
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
	
}

