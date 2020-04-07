/*
	MacBinaryDecoder.cc
	-------------------
*/

// mac-config
#include "mac_config/apple-events.hh"

// mac-file-utils
#include "mac_file/parent_directory.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

// Arcana
#include "MacBinary.hh"

// MacBinaryDecoder
#include "MacBinaryDecoder.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"


namespace MacBinaryDecoder
{
	
	namespace Ped = Pedestal;
	
	using mac::types::VRefNum_DirID;
	
	
	static
	void Decode( Io_Details::stream input, const VRefNum_DirID& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			while ( std::size_t bytes = io::read( input, data, blockSize ) )
			{
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			//std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
			Mac::SysBeep();
		}
	}
	
	static
	long OpenDocument( const Io_Details::file_spec& file )
	{
		try
		{
			VRefNum_DirID parent = mac::file::parent_directory( file );
			
			Decode( io::open_for_reading( file ), parent );
		}
		catch ( ... )
		{
			return errAEEventNotHandled;
		}
		
		return noErr;
	}
	
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
}

int main( void )
{
	using namespace MacBinaryDecoder;
	
	Ped::Application app;
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				Ped::apple_events_present);
	
	if ( apple_events_present )
	{
		mac::app::install_opendocs_handler( &OpenDocument );
	}
	
	SetCommandHandler( Ped::kCmdAbout, &About );
	
	return app.Run();
}
