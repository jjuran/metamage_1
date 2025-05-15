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
#include "scrolling.hh"
#include "string_list.hh"


#pragma exceptions off


#define PSTR_LEN( s )  ("\p" s), (sizeof s)


enum
{
	putPrompt = 3,
	putDisk   = 4,
	putLine   = 8,
	
	getDisk   =  4,
	getLine   =  9,
};

short SFSaveDisk : 0x0214;

const short n_viewable_cells = 7;

static Str27 volume_name;
static ControlRef scrollbar;
static short filename_count;
static string_list_handle filename_list;


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
void SFGet_scroll_to( const scroll_action_rec& action, short value )
{
	scroll_string_list_to( filename_list, value );
}

static const scroll_action_rec scroll_action_context =
{
	&SFGet_scroll_to,
	1,
	n_viewable_cells,
};

ModalFilterUPP gUserFilterProc;

static pascal
Boolean SFPut_filterProc( DialogRef dialog, EventRecord* event, short* itemHit )
{
	if ( basic_filterProc( dialog, event, itemHit ) )
	{
		return true;
	}
	
	return gUserFilterProc  &&  gUserFilterProc( dialog, event, itemHit );
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
	gUserFilterProc = filterProc;
	
	if ( dlgHook )
	{
		ERROR = "SFPutFile dlgHook is unimplemented";
	}
	
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
	
	DialogRef dialog = GetNewDialog( dialogID, NULL, (WindowRef) -1 );
	
	MoveWindow( dialog, where.h, where.v, false );
	
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
	
	ShowWindow( dialog );
	
	short hit = 0;
	
	do
	{
		ModalDialog( &SFPut_filterProc, &hit );
		
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
Boolean SFGet_filterProc( DialogRef dialog, EventRecord* event, short* itemHit )
{
	if ( event->what == mouseDown )
	{
		Point pt = event->where;
		GlobalToLocal( &pt );
		
		if ( short part = TestControl( scrollbar, pt ) )
		{
			switch ( part )
			{
				case kControlIndicatorPart:
					if ( TrackControl( scrollbar, pt, NULL ) )
					{
						short value = GetControlValue( scrollbar );
						
						scroll_string_list_to( filename_list, value );
					}
					break;
				
				case kControlUpButtonPart:
				case kControlDownButtonPart:
				case kControlPageUpPart:
				case kControlPageDownPart:
					TrackControl( scrollbar, pt, &scrollbar_actionProc );
					break;
			}
			
			*itemHit = getScroll;
			
			return true;
		}
	}
	
	return SFPut_filterProc( dialog, event, itemHit );
}

static inline
short scrollmax()
{
	return max( filename_count - n_viewable_cells, 0 );
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
	gUserFilterProc = filterProc;
	
	if ( dlgHook )
	{
		ERROR = "SFGetFile dlgHook is unimplemented";
	}
	
	if ( fileFilter )
	{
		ERROR = "SFGetFile fileFilter is unimplemented";
	}
	
	DialogRef dialog = GetNewDialog( dialogID, NULL, (WindowRef) -1 );
	
	MoveWindow( dialog, where.h, where.v, false );
	
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
	ValidRect( &box );
	// It's not necessary to call SetDialogItem().
	
	scrollbar[0]->contrlRfCon = (long) &scroll_action_context;
	
	GetDialogItem( dialog, getLine, &type, &h, &box );
	SetDialogItem( dialog, getLine, type, (Handle) &draw_dotted_line, &box );
	
	ShowWindow( dialog );
	
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
pascal void SFPutFile_call( Point             where,
                            ConstStr255Param  prompt,
                            ConstStr255Param  origName,
                            DlgHookUPP        dlgHook,
                            SFReply*          reply )
{
	SFPPutFile_call( where, prompt, origName, dlgHook, reply, putDlgID, NULL );
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
	SFPGetFile_call( where, prompt, fileFilter, numTypes, typeList,
	                 dlgHook, reply, getDlgID, NULL );
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
