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
			// Constructor
			Application();
			
			// Control
			int Run(); // This calls the main event loop.
	};
	
	
	void AdjustSleepForTimer( unsigned ticksToSleep );
	
	void AdjustSleepForActivity();
	
	void ScheduleImmediateEventCheck();
	
}

#endif

