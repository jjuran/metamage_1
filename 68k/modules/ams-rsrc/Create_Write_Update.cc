/*
	Create_Write_Update.cc
	----------------------
*/

#include "Create_Write_Update.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-rsrc
#include "RsrcMap.hh"
#include "rsrc_fork.hh"


#pragma exceptions off


enum
{
	resProtected = 8,
	resChanged   = 2,
};


short ResErr : 0x0A60;


static
void CreateResFile_handler( const Byte* name : __A0, short vRefNum : __D0 )
{
	OSErr err;
	
	ERROR = "CreateResFile is unimplemented";
	
	err = wrgVolTypErr;
	
	ResErr = err;
}

asm
pascal void CreateResFile_patch( const Byte* name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	MOVEQ.L  #0,D0    // vRefNum = 0
	
	JSR      CreateResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
void UpdateResFile_handler( short refnum : __D0 )
{
	ERROR = "UpdateResFile is unimplemented";
}

asm
pascal void UpdateResFile_patch( short refnum )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D0
	
	JSR      UpdateResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #2,SP
	
	JMP      (A0)
}

static
void WriteResource_handler( Handle resource : __A0 )
{
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		if ( (rsrc->attrs & (resProtected | resChanged)) != resChanged )
		{
			// noErr
		}
		else
		{
			ERROR = "WriteResource is unimplemented";
			
			ResErr = paramErr;
		}
	}
}

asm
pascal void WriteResource_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      WriteResource_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}
