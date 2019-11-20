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
#include "c_string.hh"
#include "QDGlobals.hh"


enum
{
	putPrompt = 3,
	putDisk   = 4,
	putLine   = 8,
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
	
	reply->good = hit - 2;  // $ff or $00
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
	ERROR = "SFGetFile is unimplemented";
	
	WARNING = "SFGetFile prompt: ", CSTR( prompt );
	
	reply->good = false;
	
	return userCanceledErr;
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
	ERROR = "SFPPutFile is unimplemented";
	
	WARNING = "SFPPutFile prompt:   ", CSTR( prompt );
	WARNING = "SFPPutFile origName: ", CSTR( origName );
	
	reply->good = false;
	
	return userCanceledErr;
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
	ERROR = "SFPGetFile is unimplemented";
	
	WARNING = "SFPGetFile prompt: ", CSTR( prompt );
	
	reply->good = false;
	
	return userCanceledErr;
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
