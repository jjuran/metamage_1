/*
	file_save.cc
	------------
*/

#include "file_save.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-file-utils
#include "mac_file/file_traits.hh"
#include "mac_file/open_data_fork.hh"

// mac-app-utils
#include "mac_app/file_save_dialog.hh"

// Tic-tac-toe
#include "state.hh"


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))

using mac::file::FSIORefNum;
using mac::file::file_traits;

const OSType creator = 'XvO#';
const OSType doctype = 'XvO#';

#define PROMPT "\p" "Save game as:"

template < class traits >
static
long write_and_close_stream( FSIORefNum refnum )
{
	if ( refnum < 0 )
	{
		return refnum;
	}
	
	const unsigned char* data = tictactoe::extract();
	
	Size size = *data++;
	
	Size n = traits::write( refnum, data, size );
	
	traits::close( refnum );
	
	OSErr err = n < 0    ? (OSErr) n
	          : n < size ? (OSErr) ioErr
	          :            (OSErr) noErr;
	
	return err;
}

static
OSErr create_FSRef( const FSRef&  parent,
                    CFStringRef   name,
                    OSType        creator,
                    OSType        doctype,
                    FSRef*        result )
{
	UniChar buffer[ 256 ];
	CFIndex length = CFStringGetLength( name );
	CFRange range  = CFRangeMake( 0, length );
	
	if ( length > 255 )
	{
		return errFSNameTooLong;
	}
	
	CFStringGetCharacters( name, range, buffer );
	
	FSCatalogInfo info = {};
	
	FInfo& fInfo = *(FInfo*) info.finderInfo;
	
	fInfo.fdCreator = creator;
	fInfo.fdType    = doctype;
	
	FSCatalogInfoBitmap whichInfo = kFSCatInfoFinderInfo;
	
	OSErr err = FSCreateFileUnicode( &parent,
	                                 length,
	                                 buffer,
	                                 whichInfo,
	                                 &info,
	                                 result,
	                                 NULL );
	
	/*
		In the event that a file already exists, return dupFNErr
		as we would have but also populate the output parameter.
	*/
	
	if ( err == dupFNErr )
	{
		const TextEncoding hint = kTextEncodingUnknown;
		
		err = FSMakeFSRefUnicode( &parent, length, buffer, hint, result );
		
		if ( err == noErr )
		{
			return dupFNErr;
		}
	}
	
	return err;
}

static inline
FSIORefNum FSRef_opener( const FSRef& parent, CFStringRef name )
{
	FSRef file;
	
	OSErr err = create_FSRef( parent, name, creator, doctype, &file );
	
	if ( err != noErr  &&  err != dupFNErr )
	{
		return err;
	}
	
	return mac::file::open_data_fork( file, fsRdWrPerm );
}

static
long FSRef_saver( const FSRef& parent, CFStringRef name )
{
	typedef file_traits< FSRef > traits;
	
	return write_and_close_stream< traits >( FSRef_opener( parent, name ) );
}

static inline
FSIORefNum FSSpec_opener( const FSSpec& file )
{
	OSErr err = FSpCreate( &file, creator, doctype, 0 );
	
	if ( err != noErr  &&  err != dupFNErr )
	{
		return err;
	}
	
	return mac::file::open_data_fork( file, fsRdWrPerm );
}

static
long FSSpec_saver( const FSSpec& file )
{
	typedef file_traits< FSSpec > traits;
	
	return write_and_close_stream< traits >( FSSpec_opener( file ) );
}

static inline
FSIORefNum HFS_opener( short vRefNum, long dirID, const Byte* name )
{
	OSErr err = HCreate( vRefNum, dirID, name, creator, doctype );
	
	if ( err != noErr  &&  err != dupFNErr )
	{
		return err;
	}
	
	return mac::file::open_data_fork( vRefNum, dirID, name, fsRdWrPerm );
}

static
long HFS_file_saver( short vRefNum, long dirID, const Byte* name )
{
	typedef file_traits< FSSpec > traits;
	
	FSIORefNum refnum = HFS_opener( vRefNum, dirID, name );
	
	return write_and_close_stream< traits >( refnum );
}

void file_save()
{
	using mac::app::file_save_dialog;
	
	enum
	{
		gestaltStandardFileAttr = 'stdf',
		
		gestaltStandardFile58   = 0,
	};
	
	const bool has_StandardFile_5_thru_8 =
		mac::sys::gestalt_bit_set( gestaltStandardFileAttr,
		                           gestaltStandardFile58 );
	
	if ( TARGET_API_MAC_CARBON )
	{
		file_save_dialog( doctype, creator, &FSRef_saver );
	}
	else if ( has_StandardFile_5_thru_8 )
	{
		file_save_dialog( PROMPT, "\p", &FSSpec_saver );
	}
	else
	{
		file_save_dialog( PROMPT, "\p", &HFS_file_saver );
	}
}
