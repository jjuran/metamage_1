/*
	WakeUp.hh
	---------
*/

#ifndef PEDESTAL_WAKEUP_HH
#define PEDESTAL_WAKEUP_HH


namespace Pedestal
{
	
	extern volatile bool gInWaitNextEvent;
	extern volatile bool gAsyncEventsReady;
	
	void WakeUp();
	
}

#endif

