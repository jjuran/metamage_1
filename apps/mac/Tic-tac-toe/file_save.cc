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
#include "document.hh"
#include "state.hh"
#include "window.hh"


#define POD( x )  &(x), (sizeof (x))

using mac::file::FSIORefNum;
using mac::file::file_traits;

const OSType creator = 'XvO#';
const OSType doctype = 'XvO#';

#define PROMPT "\p" "Save game as:"

#if TARGET_API_MAC_CARBON

typedef FSRef FileType;

#else

typedef FSSpec FileType;

#endif

static inline
void my_memcpy( void* dst, const void* src, size_t n )
{
	BlockMoveData( src, dst, n );
}

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

template < class File >
static inline
FSIORefNum open_file( OSErr create_err, const File& file )
{
	if ( create_err != noErr  &&  create_err != dupFNErr )
	{
		return create_err;
	}
	
	return mac::file::open_data_fork( file, fsRdWrPerm );
}

static inline
FSIORefNum FSRef_opener( const FSRef& parent, CFStringRef name, FSRef& file )
{
	OSErr err = create_FSRef( parent, name, creator, doctype, &file );
	
	return open_file( err, file );
}

static
long FSRef_saver( const FSRef& parent, CFStringRef name )
{
	typedef file_traits< FSRef > traits;
	
	FSRef file;
	
	FSIORefNum refnum = FSRef_opener( parent, name, file );
	
	long err = write_and_close_stream< traits >( refnum );
	
	if ( err == noErr )
	{
		my_memcpy( &global_document_file< FSRef >::value, POD( file ) );
	}
	
	return err;
}

static inline
FSIORefNum FSSpec_opener( const FSSpec& file )
{
	OSErr err = FSpCreate( &file, creator, doctype, 0 );
	
	return open_file( err, file );
}

static
long FSSpec_saver( const FSSpec& file )
{
	typedef file_traits< FSSpec > traits;
	
	long err = write_and_close_stream< traits >( FSSpec_opener( file ) );
	
	if ( err == noErr )
	{
		my_memcpy( &global_document_file< FSSpec >::value, POD( file ) );
	}
	
	return err;
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
	
	long err = write_and_close_stream< traits >( refnum );
	
	if ( err == noErr )
	{
		FSSpec& file = global_document_file< FSSpec >::value;
		
		file.vRefNum = vRefNum;
		file.parID   = dirID;
		
		my_memcpy( file.name, name, sizeof file.name );
	}
	
	return err;
}

void file_save()
{
	using mac::file::open_data_fork;
	
	typedef file_traits< FileType > traits;
	
	const FileType& file = global_document_file< FileType >::value;
	
	short refnum = open_data_fork( file, fsRdWrPerm );
	
	if ( write_and_close_stream< traits >( refnum ) == noErr )
	{
		document_modified = false;
	}
}

void file_save_as()
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
	
	OSStatus err;
	
	if ( TARGET_API_MAC_CARBON )
	{
		err = file_save_dialog( doctype, creator, &FSRef_saver );
	}
	else if ( has_StandardFile_5_thru_8 )
	{
		err = file_save_dialog( PROMPT, "\p", &FSSpec_saver );
	}
	else
	{
		err = file_save_dialog( PROMPT, "\p", &HFS_file_saver );
	}
	
	if ( err == noErr )
	{
		const FileType& file = global_document_file< FileType >::value;
		
		set_window_title( mac::file::get_name( file ) );
		
		document_assigned = true;
		document_modified = false;
	}
}
