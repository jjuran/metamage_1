/*	==============
 *	Application.hh
 *	==============
 */

#pragma once

// Standard C++
#include <map>

// Nitrogen
#include "Nitrogen/AppleEvents.h"
#include "Nitrogen/MacWindows.h"

// Pedestal
#include "Pedestal/ApplicationContext.hh"
#include "Pedestal/Chore.hh"
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/Menubar.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	struct AppleEventSignature
	{
		N::AEEventClass eventClass;
		N::AEEventID    eventID;
		
		AppleEventSignature()  {}
		
		AppleEventSignature( N::AEEventClass eventClass, N::AEEventID eventID )
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
			static void AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, Application* app );
			
			// Constructor & destructor.
			Application();
			
			// Control
			int Run(); // This calls the main event loop.
			
			// Event responding
			void HandleAppleEvent( const AppleEvent& appleEvent, AppleEvent& reply );
			void HandleMenuChoice( long menuChoice );
			
			bool DoCommand( MenuItemCode code );
			
			PedChoreKeeper& Repeaters()  { return myRepeaters; }
			
			DescType Class() const  { return cApplication; }
		
		protected:
			void EventLoop(); // This is the main event loop.
			
			typedef std::map< MenuItemCode, MenuItemHandler* > MenuItemHandlerMap;
			
			void RegisterMenuItemHandler( MenuItemCode code, MenuItemHandler* handler );
		
		private:
			//V::ObjectSupportLibrary myObjectSupportLib;
			
			MenuItemDispatcher menuItemDispatcher;
			MenuItemHandlerMap menuItemHandlers;
			
			MenuBar myMenubar;
			NN::Owned< N::MenuID > myAppleMenu;
			NN::Owned< N::MenuID > myFileMenu;
			NN::Owned< N::MenuID > myEditMenu;
			
			NN::Owned< N::AEEventHandler > myCoreEventsHandler;
		
			PedChoreKeeper myRepeaters;
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
	
	bool DoNextEvent( short eventMask );
	
	void ForceUpdateEvent();
	
}

