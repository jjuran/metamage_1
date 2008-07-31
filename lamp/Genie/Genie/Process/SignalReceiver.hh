/*	=================
 *	SignalReceiver.hh
 *	=================
 */

#ifndef GENIE_PROCESS_SIGNALRECEIVER_HH
#define GENIE_PROCESS_SIGNALRECEIVER_HH

// Standard C
#include <signal.h>


namespace Genie
{
	
	inline sigset_t sigset_from_signo( int signo )
	{
		return 1 << signo - 1;
	}
	
	class SignalReceiver
	{
		private:
			__sig_handler itsHandlers[ NSIG ];
			
			sigset_t  itsPendingSignals;
			sigset_t  itsBlockedSignals;
		
		public:
			SignalReceiver();
			
			__sig_handler GetSignalAction( int signo ) const;
			
			void SetSignalAction( int signo, __sig_handler action );
			
			void ResetSignalAction( int signo );
			
			bool WaitsForChildren() const;
			
			sigset_t GetPendingSignals() const  { return itsPendingSignals; }
			sigset_t GetBlockedSignals() const  { return itsBlockedSignals; }
			
			void AddPendingSignal( int sig )  { itsPendingSignals |= sigset_from_signo( sig ); }
			
			void SetBlockedSignals( sigset_t sigset )  { itsBlockedSignals = sigset; }
			
			void BlockSignals  ( sigset_t sigset )  { itsBlockedSignals |=  sigset; }
			void UnblockSignals( sigset_t sigset )  { itsBlockedSignals &= ~sigset; }
			
			bool DeliverPendingSignals();
	};
	
}

#endif

