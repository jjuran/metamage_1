/*	==============
 *	Application.hh
 *	==============
 */

#ifndef PEDESTAL_APPLICATION_HH
#define PEDESTAL_APPLICATION_HH

// Pedestal
#include "Pedestal/ApplicationContext.hh"


namespace Pedestal
{
	
	class Application : public ApplicationContext
	{
		public:
			// Constructor & destructor.
			Application();
			~Application();
			
			// Control
			int Run(); // This calls the main event loop.
	};
	
	
	// Callable at interrupt time.
	// Call from a completion routine to resume synchronous processing.
	void WakeUp();
	
	void AdjustSleepForTimer( unsigned ticksToSleep );
	
	void AdjustSleepForActivity();
	
	void ScheduleImmediateEventCheck();
	
}

#endif

