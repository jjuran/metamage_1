/*	=================
 *	SignalReceiver.hh
 *	=================
 */

#ifndef GENIE_PROCESS_SIGNALRECEIVER_HH
#define GENIE_PROCESS_SIGNALRECEIVER_HH

// Standard C
#include <signal.h>

// Genie
#include "Genie/Process/TimeKeeper.hh"


namespace Genie
{
	
	inline sigset_t sigset_from_signo( int signo )
	{
		return 1 << signo - 1;
	}
	
	class SignalReceiver : public TimeKeeper
	{
		private:
			sigset_t  itsPendingSignals;
			sigset_t  itsBlockedSignals;
		
		public:
			SignalReceiver();
			
			SignalReceiver( const SignalReceiver& other );
			
			void ClearPendingSignals()  { itsPendingSignals = 0; }
			
			sigset_t GetPendingSignals() const  { return itsPendingSignals; }
			sigset_t GetBlockedSignals() const  { return itsBlockedSignals; }
			
			void AddPendingSignal( int sig )  { itsPendingSignals |= sigset_from_signo( sig ); }
			
			void ClearPendingSignalSet( sigset_t sigset )  { itsPendingSignals &= ~sigset; }
			
			void SetBlockedSignals( sigset_t sigset )  { itsBlockedSignals = sigset; }
			
			void BlockSignals  ( sigset_t sigset )  { itsBlockedSignals |=  sigset; }
			void UnblockSignals( sigset_t sigset )  { itsBlockedSignals &= ~sigset; }
	};
	
}

#endif

