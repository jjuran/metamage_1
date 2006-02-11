/*	============
 *	GenieMain.cc
 *	============
 */

// Standard C++
#include <set>

// Nitrogen Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/AEDataModel.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Chore.hh"

// Genie
#include "Genie/Console.hh"
#include "Genie/ExecHandler.hh"
#include "Genie/ReplyHandler.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	class GenieChoreProcessTable : public Ped::PedChore
	{
		public:
			void Perform();
	};
	
	void GenieChoreProcessTable::Perform()
	{
		gProcessTable.Reap();
	}
	
	
	class TerminalsOwner
	{
		public:
			void NewWindow();
			
			void OpenDocument( const FSSpec& script );
	};
	
	
	class App : public Ped::Application,
	            public Ped::AboutBoxOwner,
	            public TerminalsOwner
	{
		private:
			Ped::AboutHandler< Ped::AboutBoxOwner > aboutHandler;
			Ped::NewHandler  < App > newHandler;
			GenieHandlerReply myReplyEventHandler;
			GenieExecHandler myExecHandler;
			NN::Owned< N::AEEventHandler > myOpenDocsEventHandler;
			GenieChoreProcessTable myChore;
		
		public:
			static void AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, App* app );
			
			App();
			~App()  { gProcessTable.KillAll(); }
			
			void HandleAppleEvent(const AppleEvent& appleEvent, AppleEvent& reply);
			void CreateSystemConsole();
			GenieProcessTable& ProcessTable()  { return gProcessTable; }
	};
	
	
	App::App()
	:
		aboutHandler( *this ),
		newHandler  ( *this ),
		myOpenDocsEventHandler
		(
			N::AEInstallEventHandler< App*, AppleEventHandler >( kCoreEventClass,
			                                                     kAEOpenDocuments,
			                                                     this )
		)
	{
		RegisterMenuItemHandler( 'abou', &aboutHandler );
		RegisterMenuItemHandler( 'new ', &newHandler   );
		
		CreateSystemConsole();
		myChore.Schedule( Repeaters() );
	}
	
	void App::AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, App* app )
	{
		app->HandleAppleEvent( appleEvent, reply );
	}
	
	void App::HandleAppleEvent(const AppleEvent& appleEvent, AppleEvent& reply)
	{
		NN::Owned< N::AEDescList > docList = N::AEGetParamDesc( appleEvent,
		                                                        keyDirectObject,
		                                                        typeAEList );
		
		int docCount = N::AECountItems( docList );
		
		for ( int index = 1; index <= docCount; ++index )
		{
			FSSpec fss = N::AEGetNthPtr< typeFSS >( docList, index );
			
			OpenDocument( fss );
		}
	}
	
	void App::CreateSystemConsole()
	{
		/*
		PedWindow* window = new PedWindow;
		mySystemConsole = window;
		GenieSystemConsole().Build(*window);
		
		SetCurrentOutputStream(window);
		*/
	}
	
	void TerminalsOwner::NewWindow()
	{
		//ConsoleProcess* terminal = new ConsoleProcess;
		SpawnNewConsole();
	}
	
	void TerminalsOwner::OpenDocument( const FSSpec& docFile )
	{
		//ConsoleProcess* terminal = new ConsoleProcess( docFile );
		SpawnNewConsole( docFile );
	}
	
}


//	main
//	----

int main()
{
	Genie::App app;
	return app.Run();
}

