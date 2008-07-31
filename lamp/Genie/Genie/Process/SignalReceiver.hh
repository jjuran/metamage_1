/*	=================
 *	SignalReceiver.hh
 *	=================
 */

#ifndef GENIE_PROCESS_SIGNALRECEIVER_HH
#define GENIE_PROCESS_SIGNALRECEIVER_HH

// Standard C++
#include <map>

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
			std::map< int, __sig_handler > itsSignalMap;
			
			sigset_t  itsPendingSignals;
			sigset_t  itsBlockedSignals;
		
		public:
			SignalReceiver();
			
			__sig_handler GetSignalAction( int signo )  { return itsSignalMap[ signo ]; }
			
			__sig_handler SetSignalAction( int signo, __sig_handler action );
			
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

