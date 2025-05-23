/*
	file_save.cc
	------------
*/

#include "file_save.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/res_error.hh"

// mac-file-utils
#include "mac_file/file_traits.hh"
#include "mac_file/Finder_info.hh"
#include "mac_file/open_data_fork.hh"

// mac-rsrc-utils
#include "mac_rsrc/create_res_file.hh"
#include "mac_rsrc/open_res_file.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/file_save_dialog.hh"

// tictactoe-logic
#include "tictactoe.hh"

// Tic-tac-toe
#include "custom_icons.hh"
#include "document.hh"
#include "set_custom_icon.hh"
#include "state.hh"
#include "window.hh"


#define UNTITLED  "Untitled"
#define EXTENSION ".tic-tac-toe"

#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  s, STRLEN( s )

#define POD( x )  &(x), (sizeof (x))

using mac::file::FSIORefNum;
using mac::file::file_traits;

const OSType creator = 'XvO#';
const OSType doctype = 'XvO#';

#define PROMPT "\p" "Save game as:"

#if TARGET_API_MAC_CARBON

typedef FSRef FileDesc;

#else

typedef FSSpec FileDesc;

#endif

static inline
void my_memcpy( void* dst, const void* src, size_t n )
{
	BlockMoveData( src, dst, n );
}

static inline
size_t ext_len_if_present( const Byte* s, const char* ext, size_t ext_len )
{
	size_t len = *s++;
	
	if ( len > ext_len  &&  memcmp( s + (len - ext_len), ext, ext_len ) == 0 )
	{
		return ext_len;
	}
	
	return 0;
}

template < class File >
static inline
void set_custom_icon_flag( const File& file )
{
	using namespace mac::file;
	
	typedef typename file_traits< File >::FileInfo FileInfo;
	
	FileInfo info;  // either FInfo or FSCatalogInfo
	
	if ( get_Finder_info( file, info ) == noErr )
	{
		UInt16& flags = Finder_flags( info );
		
		if ( (flags & kHasCustomIcon) == 0 )
		{
			flags |= kHasCustomIcon;
			
			set_Finder_info( file, info );
		}
	}
}

template < class File >
static
void set_custom_file_icon( const File& file, const Byte* data, Size size )
{
	using namespace mac::rsrc;
	
	create_res_file( file );
	
	short resfile = open_res_file( file, fsRdWrPerm );
	
	if ( resfile > 0 )
	{
		if ( set_custom_icon( data, size ) )
		{
			set_custom_icon_flag( file );
		}
		
		CloseResFile( resfile );
	}
}

template < class traits >
static
long write_and_close_stream( FSIORefNum refnum, const Byte* data, Size size )
{
	if ( refnum < 0 )
	{
		return refnum;
	}
	
	Size n = traits::write( refnum, data, size );
	
	OSErr err = n < 0    ? (OSErr) n
	          : n < size ? (OSErr) ioErr
	          :            (OSErr) noErr;
	
	if ( err == noErr )
	{
		err = traits::seteof( refnum );  // set EOF to current position
	}
	
	traits::close( refnum );
	
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
	
	const Byte* game = tictactoe::extract();
	
	Size size = *game++;
	
	long err = write_and_close_stream< traits >( refnum, game, size );
	
	if ( err == noErr )
	{
		my_memcpy( &global_document_file< FSRef >::value, POD( file ) );
		
		set_custom_file_icon( file, game, size );
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
	
	FSIORefNum refnum = FSSpec_opener( file );
	
	const Byte* game = tictactoe::extract();
	
	Size size = *game++;
	
	long err = write_and_close_stream< traits >( refnum, game, size );
	
	if ( err == noErr )
	{
		my_memcpy( &global_document_file< FSSpec >::value, POD( file ) );
		
		set_custom_file_icon( file, game, size );
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
	
	const Byte* game = tictactoe::extract();
	
	Size size = *game++;
	
	long err = write_and_close_stream< traits >( refnum, game, size );
	
	if ( err == noErr )
	{
		FSSpec& file = global_document_file< FSSpec >::value;
		
		file.vRefNum = vRefNum;
		file.parID   = dirID;
		
		my_memcpy( file.name, name, sizeof file.name );
		
		set_custom_file_icon( file, game, size );
	}
	
	return err;
}

void file_save()
{
	using mac::file::open_data_fork;
	
	typedef file_traits< FileDesc > traits;
	
	const FileDesc& file = global_document_file< FileDesc >::value;
	
	short refnum = open_data_fork( file, fsRdWrPerm );
	
	const Byte* game = tictactoe::extract();
	
	Size size = *game++;
	
	if ( write_and_close_stream< traits >( refnum, game, size ) == noErr )
	{
		set_custom_file_icon( file, game, size );
		
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
	
	const FileDesc& file = global_document_file< FileDesc >::value;
	
	OSStatus err;
	
	if ( TARGET_API_MAC_CARBON )
	{
		err = file_save_dialog( doctype, creator, &FSRef_saver );
	}
	else
	{
		const Byte* name = "\p" UNTITLED EXTENSION;
		
	#if ! TARGET_API_MAC_CARBON
		
		if ( document_assigned )
		{
			name = mac::file::get_name( file );
		}
		
	#endif
		
		const int ext_len = ext_len_if_present( name, STR_LEN( EXTENSION ) );
		
		if ( has_StandardFile_5_thru_8 )
		{
			err = file_save_dialog( PROMPT, name, &FSSpec_saver, ext_len );
		}
		else
		{
			err = file_save_dialog( PROMPT, name, &HFS_file_saver, ext_len );
		}
	}
	
	if ( err == noErr )
	{
		using mac::ui::set_window_title;
		
		set_window_title( main_window, mac::file::get_name( file ) );
		
		document_assigned = true;
		document_modified = false;
	}
}
