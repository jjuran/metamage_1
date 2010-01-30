/*	==============
 *	Application.hh
 *	==============
 */

#ifndef PEDESTAL_APPLICATION_HH
#define PEDESTAL_APPLICATION_HH

// Standard C++
#include <map>

// Mac OS Universal Interfaces
#include <AERegistry.h>

// Nitrogen
#include "Nitrogen/AppleEvents.h"
#include "Nitrogen/MacWindows.h"

// Pedestal
#include "Pedestal/ApplicationContext.hh"
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/Menubar.hh"


namespace Pedestal
{
	
	struct AppleEventSignature
	{
		Nitrogen::AEEventClass eventClass;
		Nitrogen::AEEventID    eventID;
		
		AppleEventSignature()  {}
		
		AppleEventSignature( Nitrogen::AEEventClass  eventClass,
		                     Nitrogen::AEEventID     eventID )
		:
			eventClass( eventClass ),
			eventID   ( eventID    )
		{}
	};
	
	class Application;
	
	class MenuItemDispatcher : public MenuItemHandler
	{
		private:
			Application& app;
		
		public:
			MenuItemDispatcher( Application& app ) : app( app )  {}
			
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
			
			DescType Class() const  { return cApplication; }
		
		protected:
			void EventLoop(); // This is the main event loop.
			
			typedef std::map< MenuItemCode, MenuItemHandler* > MenuItemHandlerMap;
			
			void RegisterMenuItemHandler( MenuItemCode code, MenuItemHandler* handler );
		
		private:
			MenuItemDispatcher menuItemDispatcher;
			MenuItemHandlerMap menuItemHandlers;
			
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
	
	Application& TheApp();
	
	void DispatchEvent( const EventRecord& event );
	
	// Callable at interrupt time.
	// Call from a completion routine to resume synchronous processing.
	void WakeUp();
	
	void AdjustSleepForTimer( unsigned ticksToSleep );
	
	void AdjustSleepForActivity();
	
	void ScheduleImmediateEventCheck();
	
}

#endif

