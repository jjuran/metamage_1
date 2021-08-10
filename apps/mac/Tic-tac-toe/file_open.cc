/*
	file_open.cc
	------------
*/

#include "file_open.hh"

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-file-utils
#include "mac_file/file_traits.hh"
#include "mac_file/open_data_fork.hh"

// mac-app-utils
#include "mac_app/documents.hh"
#include "mac_app/file_open_dialog.hh"

// Tic-tac-toe
#include "state.hh"
#include "window.hh"


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))

using mac::file::FSIORefNum;
using mac::file::file_traits;

static const OSType file_open_types[] = { 'XvO#' };


template < class File >
static
long file_opener( const File& file )
{
	typedef file_traits< File > traits;
	
	FSIORefNum refnum = mac::file::open_data_fork( file, fsRdPerm );
	
	if ( refnum < 0 )
	{
		return refnum;
	}
	
	UInt8 block[ 512 ];
	
	ByteCount n = traits::read( refnum, block, sizeof block );
	
	traits::close( refnum );
	
	// Read errors will appear as extreely large positive integers.
	
	if ( n <= 9 )
	{
		using namespace tictactoe;
		
		if ( restore( block, n ) )
		{
			reload( block, n );
			
			return noErr;
		}
		
		return paramErr;
	}
	
	return noErr;
}

static
long HFS_file_opener( short vRefNum, long dirID, const Byte* name )
{
	FSSpec file;
	
	file.vRefNum = vRefNum;
	file.parID   = dirID;
	
	BlockMoveData( name, file.name, 1 + name[ 0 ] );
	
	return file_opener( file );
}

void open_launched_documents()
{
	using mac::app::open_documents_with;
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				mac::sys::gestalt( 'evnt' ) != 0);
	
	if ( TARGET_API_MAC_CARBON )
	{
		open_documents_with( &file_opener< FSRef > );
	}
	else if ( apple_events_present )
	{
		open_documents_with( &file_opener< FSSpec > );
	}
	else
	{
		open_documents_with( &HFS_file_opener );
	}
}

void file_open()
{
	using mac::app::file_open_dialog;
	
	if ( TARGET_API_MAC_CARBON )
	{
		file_open_dialog( ARRAY_LEN( file_open_types ), &file_opener< FSRef > );
	}
	else
	{
		file_open_dialog( ARRAY_LEN( file_open_types ), &HFS_file_opener );
	}
}
