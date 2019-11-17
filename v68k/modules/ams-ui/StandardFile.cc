/*
	StandardFile.cc
	---------------
*/

#include "StandardFile.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __STANDARDFILE__
#include <StandardFile.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"
#include "QDGlobals.hh"

// ams-ui
#include "Dialogs.hh"
#include "string_list.hh"


#define PSTR_LEN( s )  ("\p" s), (sizeof s)


enum
{
	putPrompt = 3,
	putDisk   = 4,
	putLine   = 8,
	
	getDisk   =  4,
	getLine   =  9,
};

static const char SFPutFile_items[] =
	"\x00\x07"             // count - 1
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x4a" "\x00\x0c"  // bounds.topLeft
	"\x00\x5c" "\x00\x52"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x04" "Save"          // title
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x4a" "\x00\x72"  // bounds.topLeft
	"\x00\x5c" "\x00\xb8"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x06" "Cancel"        // title
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x0c" "\x00\x0c"  // bounds.topLeft
	"\x00\x1c" "\x00\xb8"  // bounds.bottomRight
	"\x88"                 // statText + itemDisable
	"\x00" ""              // static text
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x10" "\x00\xd1"  // bounds.topLeft
	"\x00\x22" "\x01\x27"  // bounds.bottomRight
	"\x80"                 // userItem + itemDisable
	"\x00"                 // null
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x2b" "\x00\xd9"  // bounds.topLeft
	"\x00\x3d" "\x01\x1f"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x05" "Eject"         // title
	"\x00"                 // pad
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x4a" "\x00\xd9"  // bounds.topLeft
	"\x00\x5c" "\x01\x1f"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x05" "Drive"         // title
	"\x00"                 // pad
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x22" "\x00\x0e"  // bounds.topLeft
	"\x00\x32" "\x00\xb6"  // bounds.bottomRight
	"\x90"                 // editText + itemDisable
	"\x00" ""              // initial text
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x10" "\x00\xc8"  // bounds.topLeft
	"\x00\x58" "\x00\xc9"  // bounds.bottomRight
	"\x80"                 // userItem + itemDisable
	"\x00"                 // null
	;

static const char SFGetFile_items[] =
	"\x00\x09"             // count - 1
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x1c" "\x00\x98"  // bounds.topLeft
	"\x00\x2e" "\x00\xe8"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x04" "Open"          // title
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x3b" "\x04\x80"  // bounds.topLeft
	"\x00\x4d" "\x04\xd0"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x00" ""              // title
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x5a" "\x00\x98"  // bounds.topLeft
	"\x00\x6c" "\x00\xe8"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x06" "Cancel"        // title
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x1c" "\x00\xf8"  // bounds.topLeft
	"\x00\x2e" "\x01\x58"  // bounds.bottomRight
	"\x80"                 // userItem + itemDisable
	"\x00"                 // null
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x3b" "\x01\x00"  // bounds.topLeft
	"\x00\x4d" "\x01\x50"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x05" "Eject"         // title
	"\x00"                 // pad
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x5a" "\x01\x00"  // bounds.topLeft
	"\x00\x6c" "\x01\x50"  // bounds.bottomRight
	"\x04"                 // ctrlItem + btnCtrl
	"\x05" "Drive"         // title
	"\x00"                 // pad
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x0b" "\x00\x0c"  // bounds.topLeft
	"\x00\x7d" "\x00\x7d"  // bounds.bottomRight
	"\x00"                 // userItem
	"\x00"                 // null
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x0b" "\x00\x7c"  // bounds.topLeft
	"\x00\x7d" "\x00\x8c"  // bounds.bottomRight
	"\x80"                 // userItem + itemDisable
	"\x00"                 // null
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x14" "\x00\xf4"  // bounds.topLeft
	"\x00\x74" "\x00\xf5"  // bounds.bottomRight
	"\x80"                 // userItem + itemDisable
	"\x00"                 // null
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x14" "\x04\x14"  // bounds.topLeft
	"\x00\x74" "\x04\x79"  // bounds.bottomRight
	"\x88"                 // statText + itemDisable
	"\x00" ""              // static text
	;

static inline
asm Handle PtrToHand( const void* p : __A0, long size : __D0 )
{
	DC.W     0xA9E3  // _PtrToHand
}

short SFSaveDisk : 0x0214;

const short n_viewable_cells = 7;

static Str27 volume_name;
static ControlRef scrollbar;
static short filename_count;
static string_list_handle filename_list;


static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

static inline
short max( short a, short b )
{
	return a > b ? a : b;
}

static
OSErr get_volume_name()
{
	OSErr err;
	short vRefNum;
	long freeBytes;
	
	err = GetVInfo( -SFSaveDisk, volume_name, &vRefNum, &freeBytes );
	
	if ( err == nsvErr )
	{
		SFSaveDisk = 1;
		
		err = GetVInfo( -1, volume_name, &vRefNum, &freeBytes );
	}
	
	if ( err )
	{
		ERROR = "GetVInfo returned ", err;
	}
	
	return err;
}

static
void populate_file_list( short n_types, const OSType* types )
{
	static FileParam pb;
	
	clear_string_list( filename_list );
	
	filename_count = 0;
	
	Str255 name;
	name[ 0 ] = '\0';
	
	pb.ioNamePtr = name;
	pb.ioVRefNum = -SFSaveDisk;
	pb.ioFDirIndex = 1;
	
	OSErr err;
	
	while ( (err = PBGetFInfoSync( (ParamBlockRec*) &pb )) == noErr )
	{
		++pb.ioFDirIndex;
		
		if ( name[ 0 ] == 0  ||  name[ 0 ] > 63  ||  name[ 1 ] == '.' )
		{
			continue;
		}
		
		const OSType type = pb.ioFlFndrInfo.fdType;
		
		bool match = type == '****';  // wildcard type used on non-Mac hosts
		
		for ( short i = 0;  ! match  &&  i < n_types;  ++i )
		{
			match = type == types[ i ];
		}
		
		if ( ! match )
		{
			continue;
		}
		
		append_to_string_list( filename_list, name );
		
		++filename_count;
	}
}

static
pascal void draw_disk_name( DialogRef dialog, short i )
{
	short type;
	Handle h;
	Rect box;
	
	GetDialogItem( dialog, i, &type, &h, &box );
	
	TETextBox( volume_name + 1, volume_name[ 0 ],  &box, 0 );
}

static
pascal void draw_file_list( DialogRef dialog, short i )
{
	draw_string_list( filename_list );
}

static
pascal void draw_dotted_line( DialogRef dialog, short i )
{
	short type;
	Handle h;
	Rect box;
	
	GetDialogItem( dialog, i, &type, &h, &box );
	
	QDGlobals& qd = get_QDGlobals();
	
	FillRect( &box, &qd.gray );
}

static
pascal void SFPutFile_call( Point             where,
                            ConstStr255Param  prompt,
                            ConstStr255Param  origName,
                            DlgHookUPP        dlgHook,
                            SFReply*          reply )
{
	const unsigned char* SysTwi_prompt = "\p" "File to save this new game in:";
	
	if ( fast_memcmp( prompt, SysTwi_prompt, 1 + prompt[ 0 ] ) == 0 )
	{
		/*
			Trim the prompt slightly so it fits in one line and doesn't wrap,
			since otherwise it gets clobbered by the filename edit field.
			
			Alternatively, we could dynamically widen the dialog as needed.
			The long-term solution is to implement the newer HFS SFPutFile
			dialog, which provides more space for the prompt.  But for now...
		*/
		
		prompt = "\p" "File to save this game in:";
	}
	
	const short width  = 304;
	const short height = 104;
	
	const short left = where.h;
	const short top  = where.v;
	
	Rect bounds = { top, left, top + height, left + width };
	
	Handle items = PtrToHand( SFPutFile_items, sizeof SFPutFile_items );
	
	DialogRef dialog = NewDialog( NULL,
	                              &bounds,
	                              "\p",
	                              true,
	                              dBoxProc,
	                              (WindowRef) -1,
	                              false,
	                              0,
	                              items );
	
	short type;
	Handle h;
	Rect box;
	
	get_volume_name();
	
	GetDialogItem( dialog, putPrompt, &type, &h, &box );
	SetDialogItemText( h, prompt );
	
	GetDialogItem( dialog, putDisk, &type, &h, &box );
	SetDialogItem( dialog, putDisk, type, (Handle) &draw_disk_name, &box );
	
	GetDialogItem( dialog, putEject, &type, &h, &box );
	HiliteControl( (ControlRef) h, kControlInactivePart );
	
	GetDialogItem( dialog, putName, &type, &h, &box );
	SetDialogItemText( h, origName );
	SelectDialogItemText( dialog, putName, 0, origName[ 0 ] );
	
	GetDialogItem( dialog, putLine, &type, &h, &box );
	SetDialogItem( dialog, putLine, type, (Handle) &draw_dotted_line, &box );
	
	short hit = 0;
	
	do
	{
		ModalDialog( NULL, &hit );
		
		if ( hit == 1 )
		{
			Str255 name;
			
			GetDialogItem( dialog, putName, &type, &h, &box );
			GetDialogItemText( h, name );
			
			Size len = name[ 0 ];
			
			if ( len > 0  &&  len <= sizeof reply->fName - 1 )
			{
				fast_memcpy( reply->fName, name, 1 + len );
				break;
			}
		}
		
		if ( hit == putDrive )
		{
			++SFSaveDisk;
			get_volume_name();
			
			GetDialogItem( dialog, putDisk, &type, &h, &box );
			InvalRect( &box );
		}
	}
	while ( hit != 2 );
	
	reply->good = 2 - hit;  // $01 or $00
	reply->vRefNum = -SFSaveDisk;
	reply->version = 0;
	
	DisposeDialog( dialog );
}

static pascal
void scrollbar_action( ControlRef control, short part )
{
	short value = GetControlValue( scrollbar );
	short delta = 1;
	
	/*
		kControlUpButtonPart   = 20
		kControlDownButtonPart = 21
		kControlPageUpPart     = 22
		kControlPageDownPart   = 23
	*/
	
	if ( (part & ~3) == 20 )
	{
		if ( part & 2 )
		{
			delta = n_viewable_cells;
		}
		
		if ( part & 1 )
		{
			const short maximum = GetControlMaximum( scrollbar );
			
			value = min( value + delta, maximum );
		}
		else
		{
			value = max( value - delta, 0 );
		}
	}
	
	SetControlValue( scrollbar, value );
	
	scroll_string_list_to( filename_list, value );
}

static pascal
Boolean SFGet_filterProc( DialogRef dialog, EventRecord* event, short* itemHit )
{
	DialogPeek d = (DialogPeek) dialog;
	
	if ( event->what == mouseDown )
	{
		Point pt = event->where;
		GlobalToLocal( &pt );
		
		if ( short part = TestControl( scrollbar, pt ) )
		{
			switch ( part )
			{
				case kControlIndicatorPart:
					// not yet implemented
					break;
				
				case kControlUpButtonPart:
				case kControlDownButtonPart:
				case kControlPageUpPart:
				case kControlPageDownPart:
					TrackControl( scrollbar, pt, &scrollbar_action );
					
					*itemHit = getScroll;
					return true;
			}
		}
	}
	
	return basic_filterProc( dialog, event, itemHit );
}

static inline
short scrollmax()
{
	return max( filename_count - n_viewable_cells, 0 );
}

static
pascal void SFGetFile_call( Point               where,
                            ConstStr255Param    prompt,
                            FileFilterUPP       fileFilter,
                            short               numTypes,
                            ConstSFTypeListPtr  typeList,
                            DlgHookUPP          dlgHook,
                            SFReply*            reply )
{
	const short width  = 348;
	const short height = 136;
	
	const short left = where.h;
	const short top  = where.v;
	
	Rect bounds = { top, left, top + height, left + width };
	
	Handle items = PtrToHand( SFGetFile_items, sizeof SFGetFile_items );
	
	DialogRef dialog = NewDialog( NULL,
	                              &bounds,
	                              "\p",
	                              true,
	                              dBoxProc,
	                              (WindowRef) -1,
	                              false,
	                              0,
	                              items );
	
	short type;
	Handle h;
	Rect box;
	
	get_volume_name();
	
	GetDialogItem( dialog, getDisk, &type, &h, &box );
	SetDialogItem( dialog, getDisk, type, (Handle) &draw_disk_name, &box );
	
	GetDialogItem( dialog, getEject, &type, &h, &box );
	HiliteControl( (ControlRef) h, kControlInactivePart );
	
	GetDialogItem( dialog, getNmList, &type, &h, &box );
	SetDialogItem( dialog, getNmList, type, (Handle) &draw_file_list, &box );
	
	filename_list = new_string_list( box );
	
	populate_file_list( numTypes, typeList );
	
	GetDialogItem( dialog, getScroll, &type, &h, &box );
	scrollbar = NewControl( dialog, &box, "\p", 1, 0, 0, scrollmax(), 16, 0 );
	// It's not necessary to call SetDialogItem().
	
	GetDialogItem( dialog, getLine, &type, &h, &box );
	SetDialogItem( dialog, getLine, type, (Handle) &draw_dotted_line, &box );
	
	short hit = 0;
	
	do
	{
		ModalDialog( &SFGet_filterProc, &hit );
		
		if ( hit == 1  &&  get_string_list_selection( filename_list ) )
		{
			break;
		}
		
		if ( hit == getDrive )
		{
			++SFSaveDisk;
			get_volume_name();
			
			populate_file_list( numTypes, typeList );
			
			SetControlMaximum( scrollbar, scrollmax() );
			SetControlValue( scrollbar, 0 );
			
			GetDialogItem( dialog, getDisk, &type, &h, &box );
			InvalRect( &box );
			
			GetDialogItem( dialog, getNmList, &type, &h, &box );
			InvalRect( &box );
		}
		
		if ( hit == getNmList )
		{
			Point pt;
			GetMouse( &pt );
			
			if ( bool double_clicked = string_list_click( filename_list, pt ) )
			{
				break;
			}
		}
	}
	while ( hit != 3 );
	
	reply->good = hit != 3;  // $01 or $00
	reply->vRefNum = -SFSaveDisk;
	reply->version = 0;
	
	if ( ConstStr255Param name = get_string_list_selection( filename_list ) )
	{
		fast_memcpy( reply->fName, name, 1 + name[ 0 ] );
	}
	
	dispose_string_list( filename_list );
	filename_list = NULL;
	
	scrollbar = NULL;
	
	DisposeDialog( dialog );
}

static
pascal void SFPPutFile_call( Point             where,
                             ConstStr255Param  prompt,
                             ConstStr255Param  origName,
                             DlgHookUPP        dlgHook,
                             SFReply*          reply,
                             short             dialogID,
                             ModalFilterUPP    filterProc )
{
	WARNING = "SFPPutFile is incomplete";
	
	SFPutFile_call( where, prompt, origName, dlgHook, reply );
}

static
pascal void SFPGetFile_call( Point               where,
                             ConstStr255Param    prompt,
                             FileFilterUPP       fileFilter,
                             short               numTypes,
                             ConstSFTypeListPtr  typeList,
                             DlgHookUPP          dlgHook,
                             SFReply*            reply,
                             short               dialogID,
                             ModalFilterUPP      filterProc )
{
	WARNING = "SFPGetFile is incomplete";
	
	SFGetFile_call( where, prompt, fileFilter, numTypes, typeList, dlgHook, reply );
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented Standard File call ", selector;
	
	asm { ILLEGAL }
}

asm void Pack3_patch( short selector )
{
	TST.W    SFSaveDisk
	BNE.S    valid_SFSaveDisk
	ADDQ.W   #1,SFSaveDisk
	
valid_SFSaveDisk:
	MOVEA.L  (SP)+,A0
	MOVE.W   (SP)+,D0
	MOVE.L   A0,-(SP)
	
	CMPI.W   #0x0001,D0
	BEQ      dispatch_SFPutFile
	
	CMPI.W   #0x0002,D0
	BEQ      dispatch_SFGetFile
	
	CMPI.W   #0x0003,D0
	BEQ      dispatch_SFPPutFile
	
	CMPI.W   #0x0004,D0
	BEQ      dispatch_SFPGetFile
	
	JMP      unimplemented_call
	
dispatch_SFPutFile:
	JMP      SFPutFile_call
	
dispatch_SFGetFile:
	JMP      SFGetFile_call
	
dispatch_SFPPutFile:
	JMP      SFPPutFile_call
	
dispatch_SFPGetFile:
	JMP      SFPGetFile_call
}
