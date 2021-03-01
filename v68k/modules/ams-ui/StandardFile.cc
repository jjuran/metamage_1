/*
	StandardFile.cc
	---------------
*/

#include "StandardFile.hh"

// Mac OS
#ifndef __STANDARDFILE__
#include <StandardFile.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"
#include "QDGlobals.hh"


enum
{
	putPrompt = 3,
	putDisk   = 4,
	putLine   = 8,
	
	getDisk   =  4,
	getLine   =  9,
	getPrompt = 10,
	getName   = 11,
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
	"\x00\x0a"             // count - 1
	
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
	"\x80"                 // userItem + itemDisable
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
	
	"\x00\x00\x00\x00"     // handle placeholder
	"\x00\x3c" "\x00\x0e"  // bounds.topLeft
	"\x00\x4c" "\x00\xe5"  // bounds.bottomRight
	"\x90"                 // editText + itemDisable
	"\x00" ""              // initial text
	;

static inline
asm Handle PtrToHand( const void* p : __A0, long size : __D0 )
{
	DC.W     0xA9E3  // _PtrToHand
}

short SFSaveDisk : 0x0214;

static
pascal void draw_disk_name( DialogRef dialog, short i )
{
	OSErr err;
	Str27 name;
	short vRefNum;
	long freeBytes;
	
	if ( SFSaveDisk )
	{
		err = GetVInfo( -SFSaveDisk, name, &vRefNum, &freeBytes );
	}
	
	if ( ! SFSaveDisk  ||  err == nsvErr )
	{
		SFSaveDisk = 1;
		
		err = GetVInfo( -1, name, &vRefNum, &freeBytes );
	}
	
	if ( err )
	{
		ERROR = "GetVInfo returned ", err;
		return;
	}
	
	short type;
	Handle h;
	Rect box;
	
	GetDialogItem( dialog, i, &type, &h, &box );
	
	TETextBox( name + 1, name[ 0 ],  &box, 0 );
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
pascal OSErr SFPutFile_call( Point             where,
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
	
	GetDialogItem( dialog, putPrompt, &type, &h, &box );
	SetDialogItemText( h, prompt );
	
	GetDialogItem( dialog, putDisk, &type, &h, &box );
	SetDialogItem( dialog, putDisk, type, (Handle) &draw_disk_name, &box );
	
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
			
			GetDialogItem( dialog, putDisk, &type, &h, &box );
			InvalRect( &box );
		}
	}
	while ( hit != 2 );
	
	reply->good = 2 - hit;  // $01 or $00
	reply->vRefNum = -SFSaveDisk;
	reply->version = 0;
	
	DisposeDialog( dialog );
	
	return noErr;
}

static
pascal OSErr SFGetFile_call( Point               where,
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
	
	GetDialogItem( dialog, getDisk, &type, &h, &box );
	SetDialogItem( dialog, getDisk, type, (Handle) &draw_disk_name, &box );
	
	GetDialogItem( dialog, getLine, &type, &h, &box );
	SetDialogItem( dialog, getLine, type, (Handle) &draw_dotted_line, &box );
	
	GetDialogItem( dialog, getPrompt, &type, &h, &box );
	box.left  =  12;
	box.right = 140;
	SetDialogItem( dialog, getPrompt, type, h, &box );
	SetDialogItemText( h, "\p" "Name of file to open:" );
	
	short hit = 0;
	
	do
	{
		ModalDialog( NULL, &hit );
		
		if ( hit == 1 )
		{
			Str255 name;
			
			GetDialogItem( dialog, getName, &type, &h, &box );
			GetDialogItemText( h, name );
			
			Size len = name[ 0 ];
			
			if ( len > 0  &&  len <= sizeof reply->fName - 1 )
			{
				fast_memcpy( reply->fName, name, 1 + len );
				break;
			}
		}
		
		if ( hit == getDrive )
		{
			++SFSaveDisk;
			
			GetDialogItem( dialog, getDisk, &type, &h, &box );
			InvalRect( &box );
		}
	}
	while ( hit != 3 );
	
	reply->good = hit != 3;  // $01 or $00
	reply->vRefNum = -SFSaveDisk;
	reply->version = 0;
	
	DisposeDialog( dialog );
	
	return noErr;
}

static
pascal OSErr SFPPutFile_call( Point             where,
                              ConstStr255Param  prompt,
                              ConstStr255Param  origName,
                              DlgHookUPP        dlgHook,
                              SFReply*          reply,
                              short             dialogID,
                              ModalFilterUPP    filterProc )
{
	WARNING = "SFPPutFile is incomplete";
	
	return SFPutFile_call( where, prompt, origName, dlgHook, reply );
}

static
pascal OSErr SFPGetFile_call( Point               where,
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
	
	return SFGetFile_call( where, prompt, fileFilter, numTypes, typeList, dlgHook, reply );
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented Standard File call ", selector;
	
	asm { ILLEGAL }
}

asm void Pack3_patch( short selector )
{
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
