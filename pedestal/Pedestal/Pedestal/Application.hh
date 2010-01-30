/*	==============
 *	Application.hh
 *	==============
 */

#ifndef PEDESTAL_APPLICATION_HH
#define PEDESTAL_APPLICATION_HH

// Nitrogen
#include "Nitrogen/AppleEvents.h"

// Pedestal
#include "Pedestal/ApplicationContext.hh"
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	class MenuItemHandler
	{
		public:
			virtual bool Run( MenuItemCode code ) const = 0;
	};
	
	class Application : public ApplicationContext
	{
		public:
			// Constructor & destructor.
			Application();
			~Application();
			
			// Control
			int Run(); // This calls the main event loop.
			
			// Event responding
			void HandleAppleEvent( const Nitrogen::AppleEvent&  appleEvent,
			                       Nitrogen::AppleEvent&        reply );
			
			void HandleMenuChoice( long menuChoice );
			
			bool DoCommand( MenuItemCode code );
		
		protected:
			void EventLoop(); // This is the main event loop.
			
			void RegisterMenuItemHandler( MenuItemCode code, MenuItemHandler* handler );
	};
	
	template < class Responder >
	class AboutHandler : public MenuItemHandler
	{
		private:
			Responder& responder;
		
		public:
			AboutHandler( Responder& responder ) : responder( responder )  {}
			
			bool Run( MenuItemCode ) const
			{
				responder.ShowAboutBox();
				return true;
			}
	};
	
	template < class Responder >
	class NewHandler : public MenuItemHandler
	{
		private:
			Responder& responder;
		
		public:
			NewHandler( Responder& responder ) : responder( responder )  {}
			
			bool Run( MenuItemCode ) const
			{
				responder.NewWindow();
				return true;
			}
	};
	
	// Callable at interrupt time.
	// Call from a completion routine to resume synchronous processing.
	void WakeUp();
	
	void AdjustSleepForTimer( unsigned ticksToSleep );
	
	void AdjustSleepForActivity();
	
	void ScheduleImmediateEventCheck();
	
}

#endif

