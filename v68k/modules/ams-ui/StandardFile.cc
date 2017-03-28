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


static
pascal OSErr SFPutFile_call( Point             where,
                             ConstStr255Param  prompt,
                             ConstStr255Param  origName,
                             DlgHookUPP        dlgHook,
                             SFReply*          reply )
{
	ERROR = "SFPutFile is unimplemented";
	
	WARNING = "SFPutFile prompt:   ", CSTR( prompt );
	WARNING = "SFPutFile origName: ", CSTR( origName );
	
	reply->good = false;
	
	return userCanceledErr;
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
