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
#include "Pedestal/Menubar.hh"


namespace Pedestal
{
	
	class MenuItemDispatcher : public MenuItemHandler
	{
		public:
			bool Run( MenuItemCode code ) const;
	};
	
	class Application : public ApplicationContext
	{
		public:
			static void AppleEventHandler( const Nitrogen::AppleEvent&  appleEvent,
			                               Nitrogen::AppleEvent&        reply,
			                               Application*                 app );
			
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
		
		private:
			MenuItemDispatcher menuItemDispatcher;
			
			MenuBar myMenubar;
			Nucleus::Owned< Nitrogen::MenuID > myAppleMenu;
			Nucleus::Owned< Nitrogen::MenuID > myFileMenu;
			Nucleus::Owned< Nitrogen::MenuID > myEditMenu;
			
			Nucleus::Owned< Nitrogen::AEEventHandler > myCoreEventsHandler;
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

