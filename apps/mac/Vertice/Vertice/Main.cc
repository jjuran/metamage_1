/*
	Vertice/Main.cc
	---------------
*/

// mac-app-utils
#include "mac_app/event_handlers.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"

// Vertice
#include "Vertice/Document.hh"


namespace Vertice
{
	
	namespace Ped = Pedestal;
	
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	static
	long open_doc( const FSSpec& file )
	{
		try
		{
			OpenDocument( file );
		}
		catch ( ... )
		{
			return -1;
		}
		
		return 0;
	}
	
}


int main(void)
{
	using namespace Vertice;
	
	Ped::Application app;
	
	mac::app::install_opendocs_handler( &open_doc );
	
	SetCommandHandler( Ped::kCmdAbout, &About );
	
	return app.Run();
}
