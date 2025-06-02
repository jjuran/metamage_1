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
#include "mac_file/parent_directory.hh"

// mac-app-utils
#include "mac_app/documents.hh"
#include "mac_app/file_open_dialog.hh"

// MacBinaryDecoder
#include "decode.hh"


#pragma exceptions off


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))

using mac::types::VRefNum_DirID;

using mac::file::file_traits;

static const OSType file_open_types[] = { 'mBIN', 'BIN+' };


#if TARGET_API_MAC_CARBON
	
	typedef FSRef FileSpec;
	
#else
	
	typedef FSSpec FileSpec;
	
#endif

static
long file_opener( const FileSpec& file )
{
	typedef file_traits< FileSpec > traits;
	
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

void open_launched_documents()
{
	using mac::app::open_documents_with;
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				mac::sys::gestalt( 'evnt' ) != 0);
	
	if ( TARGET_API_MAC_CARBON  ||  apple_events_present )
	{
		open_documents_with( &file_opener );
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
		file_open_dialog( ARRAY_LEN( file_open_types ), &file_opener );
	}
	else
	{
		file_open_dialog( ARRAY_LEN( file_open_types ), &HFS_file_opener );
	}
}
