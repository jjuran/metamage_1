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
	
	class SignalReceiver
	{
		public:
			std::map< int, __sig_handler > itsSignalMap;
			
			sigset_t  itsPendingSignals;
			sigset_t  itsBlockedSignals;
			sigset_t  itsMaskedSignals;
		
		public:
			SignalReceiver();
			
			__sig_handler SetSignalAction( int signal, __sig_handler action );
			
			bool DeliverPendingSignals();
	};
	
}

#endif

