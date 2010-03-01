/*	===============
 *	Vertice/Main.cc
 *	===============
 */

// Nitrogen
#include "Nitrogen/AEDataModel.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"
#include "Pedestal/WindowsOwner.hh"

// Vertice
#include "Vertice/Document.hh"


namespace Vertice
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	class DocumentsOwner : public Ped::WindowsOwner
	{
		public:
			void OpenDocument( const FSSpec& file );
	};
	
	class App : public Ped::Application,
	            public DocumentsOwner
	{
		private:
			//PedMenu myOptionsMenu;
			//PedMenu myViewMenu;
			n::owned< N::AEEventHandler > myOpenDocsEventHandler;
		
		public:
			static void AppleEventHandler( N::AppleEvent const&  appleEvent,
			                               N::AppleEvent      &  reply,
			                               App*                  app );
			
			App();
			
			void HandleAppleEvent( const N::AppleEvent& appleEvent, N::AppleEvent& reply );
	};
	
	
	void DocumentsOwner::OpenDocument( const FSSpec& file )
	{
		Window* doc = new Window( file.name );
		
		boost::shared_ptr< Ped::Window > document( doc );
		
		AddWindow( document );
		
		doc->Load( file );
	}
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	App::App()
	:
		myOpenDocsEventHandler
		(
			N::AEInstallEventHandler< App*, AppleEventHandler >( N::kCoreEventClass,
			                                                     N::kAEOpenDocuments,
			                                                     this ) )
	//, myOptionsMenu(131)
	//, myViewMenu(132)
	{
		SetCommandHandler( Ped::kCmdAbout, &About );
	}
	
	void App::AppleEventHandler( const N::AppleEvent& appleEvent, N::AppleEvent& reply, App* app )
	{
		app->HandleAppleEvent( appleEvent, reply );
	}
	
	void App::HandleAppleEvent( const N::AppleEvent& appleEvent, N::AppleEvent& reply )
	{
		n::owned< N::AEDescList_Data > docList = N::AEGetParamDesc( appleEvent,
		                                                            N::keyDirectObject,
		                                                            N::typeAEList );
		
		int docCount = N::AECountItems( docList );
		
		for ( int index = 1;  index <= docCount;  index++ )
		{
			FSSpec fss = N::AEGetNthPtr< N::typeFSS >( docList, index );
			
			OpenDocument( fss );
		}
	}
	
}


int main(void)
{
	Vertice::App app;
	
	return app.Run();
}

