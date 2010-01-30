/*	==============
 *	Application.hh
 *	==============
 */

#ifndef PEDESTAL_APPLICATION_HH
#define PEDESTAL_APPLICATION_HH


namespace Pedestal
{
	
	class Application
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

