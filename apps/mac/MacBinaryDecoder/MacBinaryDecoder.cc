/*
	MacBinaryDecoder.cc
	-------------------
*/

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/beep.hh"

// mac-file-utils
#include "mac_file/parent_directory.hh"

// mac-app-utils
#include "mac_app/documents.hh"
#include "mac_app/file_open_dialog.hh"

// Arcana
#include "MacBinary.hh"

// MacBinaryDecoder
#include "MacBinaryDecoder.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))


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
			mac::sys::beep();
		}
	}
	
	static
	long file_opener( const Io_Details::file_spec& file )
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
	
	static
	long HFS_file_opener( short vRefNum, long dirID, const Byte* name )
	{
	#if ! TARGET_API_MAC_CARBON
		
		FSSpec file;
		
		file.vRefNum = vRefNum;
		file.parID   = dirID;
		
		BlockMoveData( name, file.name, 1 + name[ 0 ] );
		
		return file_opener( file );
		
	#endif
		
		return 0;
	}
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	static const OSType file_open_types[] = { 'mBIN', 'BIN+' };
	
	static
	bool FileOpenDialog( Ped::CommandCode )
	{
		using mac::app::file_open_dialog;
		using Ped::apple_events_present;
		
		if ( apple_events_present )
		{
			file_open_dialog( ARRAY_LEN( file_open_types ), &file_opener );
		}
		else
		{
			file_open_dialog( ARRAY_LEN( file_open_types ), &HFS_file_opener );
		}
		
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
		mac::app::open_documents_with( &file_opener );
	}
	else
	{
		mac::app::open_documents_with( &HFS_file_opener );
	}
	
	SetCommandHandler( Ped::kCmdAbout, &About );
	SetCommandHandler( Ped::kCmdOpen,  &FileOpenDialog );
	
	return app.Run();
}
