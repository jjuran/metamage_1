/*	===============
 *	Vertice/Main.cc
 *	===============
 */

// Nitrogen
#include "Nitrogen/AEDataModel.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"

// Vertice
#include "Vertice/Document.hh"


namespace Vertice
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	class DocumentsOwner : public Ped::WindowsOwner< Window >
	{
		public:
			void OpenDocument( const FSSpec& file );
	};
	
	class App : public Ped::Application,
	            public Ped::AboutBoxOwner,
	            public DocumentsOwner
	{
		private:
			Ped::AboutHandler< App > aboutHandler;
			//PedMenu myOptionsMenu;
			//PedMenu myViewMenu;
			NN::Owned< N::AEEventHandler > myOpenDocsEventHandler;
		
		public:
			static void AppleEventHandler( AppleEvent const&  appleEvent,
			                               AppleEvent      &  reply,
			                               App*               app );
			
			App();
			
			void HandleAppleEvent( const AppleEvent& appleEvent, AppleEvent& reply );
	};
	
	
	void DocumentsOwner::OpenDocument( const FSSpec& file )
	{
		Window* doc = new Window( fClosure, file.name );
		
		fWindows[ doc->Get() ] = boost::shared_ptr< Window >( doc );
		
		doc->Load( file );
	}
	
	App::App()
	:
		aboutHandler( *this ),
		myOpenDocsEventHandler
		(
			N::AEInstallEventHandler< App*, AppleEventHandler >( kCoreEventClass,
			                                                     kAEOpenDocuments,
			                                                     this ) )
	//, myOptionsMenu(131)
	//, myViewMenu(132)
	{
		RegisterMenuItemHandler( 'abou', &aboutHandler );
	//	RegisterMenuItemHandler( 'new ', &newHandler   );  // New is not supported
		//myMenubar.AddMenu(myOptionsMenu);
		//myMenubar.AddMenu(myViewMenu);
	}
	
	void App::AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, App* app )
	{
		app->HandleAppleEvent( appleEvent, reply );
	}
	
	void App::HandleAppleEvent( const AppleEvent& appleEvent, AppleEvent& reply )
	{
		NN::Owned< N::AEDescList > docList = N::AEGetParamDesc( appleEvent,
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

