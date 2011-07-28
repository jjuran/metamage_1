/*
	Vertice/Main.cc
	---------------
*/

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AppleEvents.hh"

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
	
	static DocumentsOwner gDocuments;
	
	class App : public Ped::Application
	{
		public:
			static void AppleEventHandler( Mac::AppleEvent const&  appleEvent,
			                               Mac::AppleEvent      &  reply );
			
			App();
	};
	
	
	void DocumentsOwner::OpenDocument( const FSSpec& file )
	{
		Window* doc = new Window( file.name );
		
		boost::intrusive_ptr< Ped::Window > document( doc );
		
		AddWindow( document );
		
		doc->Load( file );
	}
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	App::App()
	{
		N::AEInstallEventHandler< AppleEventHandler >( Mac::kCoreEventClass,
		                                               Mac::kAEOpenDocuments ).release();
		
		SetCommandHandler( Ped::kCmdAbout, &About );
	}
	
	void App::AppleEventHandler( const Mac::AppleEvent& appleEvent, Mac::AppleEvent& reply )
	{
		n::owned< Mac::AEDescList_Data > docList = N::AEGetParamDesc( appleEvent,
		                                                              Mac::keyDirectObject,
		                                                              Mac::typeAEList );
		
		int docCount = N::AECountItems( docList );
		
		for ( int index = 1;  index <= docCount;  index++ )
		{
			FSSpec fss = N::AEGetNthPtr< Mac::typeFSS >( docList, index );
			
			gDocuments.OpenDocument( fss );
		}
	}
	
}


int main(void)
{
	Vertice::App app;
	
	return app.Run();
}

