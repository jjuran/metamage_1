/*
	Vertice/Main.cc
	---------------
*/

// mac-config
#include "mac_config/apple-events.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"
#include "mac_app/file_open_dialog.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"

// Vertice
#include "Vertice/Document.hh"


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))


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
	
	static const mac::app::OSType file_open_types[] = { 'TEXT' };
	
	static
	bool Open( Ped::CommandCode )
	{
		mac::app::file_open_dialog( ARRAY_LEN( file_open_types ), &open_doc );
		
		return true;
	}
	
}


int main(void)
{
	using namespace Vertice;
	
	Ped::Application app;
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				Ped::apple_events_present);
	
	if ( apple_events_present )
	{
		mac::app::install_opendocs_handler( &open_doc );
	}
	
	SetCommandHandler( Ped::kCmdAbout, &About );
	SetCommandHandler( Ped::kCmdOpen,  &Open  );
	
	return app.Run();
}
