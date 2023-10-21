/*
	Create_Write_Update.cc
	----------------------
*/

#include "Create_Write_Update.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"

// ams-rsrc
#include "RsrcMap.hh"
#include "rsrc_fork.hh"


#pragma exceptions off


enum
{
	resProtected = 8,
	resChanged   = 2,
};

enum
{
	mapReadOnly = 128,
};


short ResErr : 0x0A60;


struct empty_resource_map : rsrc_map_header
{
	UInt16 empty_type_list;
};

static
void CreateResFile_handler( const Byte* name : __A0, short vRefNum : __D0 )
{
	OSErr err;
	short refNum;
	
	err = Create( name, vRefNum, 'RSED', 'rsrc' );
	
	if ( err != noErr  &&  err != dupFNErr )
	{
		goto bail;
	}
	
	err = OpenRF( name, vRefNum, &refNum );
	
	if ( err != noErr )
	{
		goto bail;
	}
	
	SInt32 rsrc_fork_size;
	
	err = GetEOF( refNum, &rsrc_fork_size );
	
	if ( err == noErr  &&  rsrc_fork_size > 0 )
	{
		err = dupFNErr;
	}
	
	if ( err != noErr )
	{
		goto close_and_bail;
	}
	
	enum
	{
		offset_to_data = sizeof (rsrc_fork_superheader),
		length_of_data = 0,
		
		offset_to_map = offset_to_data + length_of_data,
		length_of_map = sizeof (empty_resource_map),
		
		offset_to_types = sizeof (rsrc_map_header),
		offset_to_names = sizeof (empty_resource_map),
		empty_type_list = -1,
	};
	
	empty_resource_map empty_map;
	
	fast_memset( &empty_map, '\0', sizeof empty_map );
	
	rsrc_fork_header& fork_header = empty_map.fork_header;
	
	fork_header.offset_to_data = offset_to_data;  // 256
	fork_header.offset_to_map  = offset_to_map;   // 256
//	fork_header.length_of_data = length_of_data;  //   0
	fork_header.length_of_map  = length_of_map;   //  30
	
//	empty_map.next_map = NULL;
//	empty_map.refnum   = 0;
//	empty_map.attrs    = 0;
	empty_map.offset_to_types = offset_to_types;  // 28 bytes
	empty_map.offset_to_names = offset_to_names;  // 30 bytes
	empty_map.empty_type_list = empty_type_list;  //  0 types
	
	Size fork_header_size = sizeof (rsrc_fork_header);
	Size size_of_rsrc_map = length_of_map;
	
	(err = SetEOF ( refNum, 286 ))                                    ||
	(err = FSWrite( refNum, &fork_header_size, (Ptr) &fork_header ))  ||
	(err = SetFPos( refNum, fsFromStart, 256 ))                       ||
	(err = FSWrite( refNum, &size_of_rsrc_map, (Ptr) &empty_map   ));
	
	if ( err != noErr )
	{
		goto close_and_bail;
	}
	
close_and_bail:
	FSClose( refNum );
	
bail:
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
