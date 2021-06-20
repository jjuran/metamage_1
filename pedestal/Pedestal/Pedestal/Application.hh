/*	==============
 *	Application.hh
 *	==============
 */

#ifndef PEDESTAL_APPLICATION_HH
#define PEDESTAL_APPLICATION_HH


namespace Pedestal
{
	
	extern const bool apple_events_present;
	
	extern void (*gThreadYield_Hook)();
	extern bool (*gActivelyBusy_Hook)();
	extern bool (*gReadyToExit_Hook)();
	
	void set_key_event_tap( bool enabled );
	
	class Application
	{
		public:
			// Constructor
			Application();
			
			// Control
			int Run(); // This calls the main event loop.
	};
	
	
	void AdjustSleepForTimer( unsigned ticksToSleep );
	
}

#endif
