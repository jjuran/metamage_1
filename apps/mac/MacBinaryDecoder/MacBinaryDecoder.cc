/*
	MacBinaryDecoder.cc
	-------------------
*/

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/beep.hh"

// mac-file-utils
#include "mac_file/file_traits.hh"
#include "mac_file/open_data_fork.hh"
#include "mac_file/parent_directory.hh"

// mac-app-utils
#include "mac_app/documents.hh"
#include "mac_app/file_open_dialog.hh"

// Arcana
#include "MacBinary.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))


using mac::file::FSIORefNum;
using mac::file::file_traits;

#if TARGET_API_MAC_CARBON
	
	typedef FSRef FileSpec;
	
#else
	
	typedef FSSpec FileSpec;
	
#endif

static
long read_FSRead( FSIORefNum refnum, void* buffer, SInt32 n_bytes )
{
	OSErr err = FSRead( refnum, &n_bytes, buffer );
	
	if ( err  &&  err != eofErr )
	{
		return err;
	}
	
	return n_bytes;
}

namespace MacBinaryDecoder
{
	
	namespace Ped = Pedestal;
	
	using mac::types::VRefNum_DirID;
	
	
	static
	OSErr Decode( FSIORefNum input, const VRefNum_DirID& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			long bytes;
			
			while ( (bytes = read_FSRead( input, data, blockSize )) > 0 )
			{
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
			
			if ( bytes < 0 )
			{
				return bytes;
			}
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			//std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
			mac::sys::beep();
		}
		
		return noErr;
	}
	
	static
	long file_opener( const FileSpec& file )
	{
		typedef file_traits< FileSpec > traits;
		
		try
		{
			VRefNum_DirID parent = mac::file::parent_directory( file );
			
			FSIORefNum opened = mac::file::open_data_fork( file, fsRdPerm );
			
			if ( opened < 0 )
			{
				return opened;
			}
			
			OSErr err = Decode( opened, parent );
			
			traits::close( opened );
			
			return err;
		}
		catch ( ... )
		{
			return errAEEventNotHandled;
		}
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
