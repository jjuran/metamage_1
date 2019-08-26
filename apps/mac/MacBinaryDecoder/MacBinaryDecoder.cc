/*
	MacBinaryDecoder.cc
	-------------------
*/

// mac-app-utils
#include "mac_app/event_handlers.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/Files.hh"

// Arcana
#include "MacBinary.hh"

// MacBinaryDecoder
#include "MacBinaryDecoder.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"


namespace MacBinaryDecoder
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static void Decode( Io_Details::stream input, const N::FSDirSpec& destDir )
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
			N::FSDirSpec parent = n::convert< N::FSDirSpec >( io::get_preceding_directory( file ) );
			
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
	
	App::App()
	{
		mac::app::install_opendocs_handler( &OpenDocument );
		
		SetCommandHandler( Ped::kCmdAbout, &About );
	}
	
}

int main( void )
{
	MacBinaryDecoder::App app;
	
	return app.Run();
}
