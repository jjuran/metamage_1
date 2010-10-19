/*	============
 *	GenieMain.cc
 *	============
 */

// Nitrogen
#include "Nitrogen/AEDataModel.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"

// Genie
#include "Genie/AboutBox.hh"
#include "Genie/ReplyHandler.hh"
#include "Genie/ProcessList.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	class TerminalsOwner
	{
		public:
			void NewWindow();
			
			void OpenDocument( const FSSpec& script );
	};
	
	static TerminalsOwner gTerminals;
	
	
	class App : public Ped::Application
	{
		private:
			GenieHandlerReply myReplyEventHandler;
			n::owned< N::AEEventHandler > myOpenDocsEventHandler;
		
		public:
			static void AppleEventHandler( const N::AppleEvent& appleEvent, N::AppleEvent& reply, App* app );
			
			App();
			~App()  { kill_all_processes(); }
			
			void HandleAppleEvent( const N::AppleEvent& appleEvent, N::AppleEvent& reply );
			void CreateSystemConsole();
	};
	
	
	static bool About( Ped::CommandCode )
	{
		ShowAboutBox();
		
		return true;
	}
	
	static bool NewDocument( Ped::CommandCode )
	{
		gTerminals.NewWindow();
		
		return true;
	}
	
	App::App()
	:
		myOpenDocsEventHandler
		(
			N::AEInstallEventHandler< App*, AppleEventHandler >( N::kCoreEventClass,
			                                                     N::kAEOpenDocuments,
			                                                     this )
		)
	{
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
		CreateSystemConsole();
	}
	
	void App::AppleEventHandler( const N::AppleEvent& appleEvent, N::AppleEvent& reply, App* app )
	{
		app->HandleAppleEvent( appleEvent, reply );
	}
	
	void App::HandleAppleEvent( const N::AppleEvent& appleEvent, N::AppleEvent& /*reply*/ )
	{
		n::owned< N::AEDescList_Data > docList = N::AEGetParamDesc( appleEvent,
		                                                            N::keyDirectObject,
		                                                            N::typeAEList );
		
		int docCount = N::AECountItems( docList );
		
		for ( int index = 1; index <= docCount; ++index )
		{
			FSSpec fss = N::AEGetNthPtr< N::typeFSS >( docList, index );
			
			gTerminals.OpenDocument( fss );
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
		spawn_process( "/bin/jgetty" );
	}
	
	void TerminalsOwner::OpenDocument( const FSSpec& docFile )
	{
		//ConsoleProcess* terminal = new ConsoleProcess( docFile );
		//SpawnNewConsole( docFile );
	}
	
}


//	main
//	----

int main()
{
	Genie::App app;
	
	return app.Run();
}

