/*
	MacBinaryDecoder.cc
	-------------------
*/

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"

// MacBinaryDecoder
#include "file_open.hh"


namespace Ped = Pedestal;

static bool About( Ped::CommandCode )
{
	Ped::ShowAboutBox();
	
	return true;
}

static
bool FileOpenDialog( Ped::CommandCode )
{
	file_open();
	
	return true;
}

int main( void )
{
	Ped::Application app;
	
	open_launched_documents();
	
	SetCommandHandler( Ped::kCmdAbout, &About );
	SetCommandHandler( Ped::kCmdOpen,  &FileOpenDialog );
	
	return app.Run();
}
