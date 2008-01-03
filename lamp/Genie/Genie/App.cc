/*	============
 *	GenieMain.cc
 *	============
 */

// Nitrogen
#include "Nitrogen/AEDataModel.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"

// Genie
#include "Genie/Console.hh"
#include "Genie/ExecHandler.hh"
#include "Genie/ReplyHandler.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
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
		
		public:
			static void AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, App* app );
			
			App();
			~App()  { GetProcessList().KillAll(); }
			
			void HandleAppleEvent(const AppleEvent& appleEvent, AppleEvent& reply);
			void CreateSystemConsole();
	};
	
	
	App::App()
	:
		aboutHandler( *this ),
		newHandler  ( *this ),
		myOpenDocsEventHandler
		(
			N::AEInstallEventHandler< App*, AppleEventHandler >( N::kCoreEventClass,
			                                                     N::kAEOpenDocuments,
			                                                     this )
		)
	{
		RegisterMenuItemHandler( 'abou', &aboutHandler );
		RegisterMenuItemHandler( 'new ', &newHandler   );
		
		CreateSystemConsole();
	}
	
	void App::AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, App* app )
	{
		app->HandleAppleEvent( appleEvent, reply );
	}
	
	void App::HandleAppleEvent( const AppleEvent& appleEvent, AppleEvent& /*reply*/ )
	{
		NN::Owned< N::AEDescList > docList = N::AEGetParamDesc( appleEvent,
		                                                        N::keyDirectObject,
		                                                        N::typeAEList );
		
		int docCount = N::AECountItems( docList );
		
		for ( int index = 1; index <= docCount; ++index )
		{
			FSSpec fss = N::AEGetNthPtr< N::typeFSS >( docList, index );
			
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
	
	Genie::InitKernelModules();
	
	return app.Run();
}

