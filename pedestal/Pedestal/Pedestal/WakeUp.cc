/*
	WakeUp.cc
	---------
*/

#include "Pedestal/WakeUp.hh"

// Nitrogen
#include "Nitrogen/Processes.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	volatile bool gInWaitNextEvent  = false;
	volatile bool gAsyncEventsReady = false;
	
	
	static ProcessSerialNumber gOurPSN = N::GetCurrentProcess();
	
	
	void WakeUp()
	{
		if ( gInWaitNextEvent )
		{
			const OSErr err = ::WakeUpProcess( &gOurPSN );
			
			if ( err != noErr )
			{
				// FIXME:  Spawn a Time Manager task to retry later
			}
		}
		else
		{
			gAsyncEventsReady = true;
		}
	}
	
}

