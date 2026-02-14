/*
	Open_Close.cc
	-------------
*/

#include "Open_Close.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// ams-common
#include "master_pointer.hh"
#include "unglue.hh"

// ams-rsrc
#include "Create_Write_Update.hh"
#include "get_FCB.hh"
#include "RsrcMap.hh"
#include "rsrc_fork.hh"


#pragma exceptions off


enum
{
	mapReadOnly = 128,
};

short MemErr      : 0x0220;
short CurApRefNum : 0x0900;
Handle TopMapHndl : 0x0A50;
short CurMap      : 0x0A5A;
short ResErr      : 0x0A60;


static
short OpenResFile_handler( const Byte* name : __A0, short vRefNum : __D0 )
{
	OSErr err;
	short refNum;
	
	err = OpenRF( name, vRefNum, &refNum );
	
	if ( err != noErr )
	{
		goto bail;
	}
	
	FCB* fcb = get_FCB( refNum );
	
	Ptr fork_buffer = fcb->fcbBfAdr;
	
	if ( fcb->fcbEOF < 256 )  // TODO
	{
		err = eofErr;
		goto close_and_bail;
	}
	
	rsrc_fork_header& header = *(rsrc_fork_header*) fork_buffer;
	
	Ptr start_of_map = fork_buffer + header.offset_to_map;
	
	Handle h = PtrToHand( start_of_map, header.length_of_map );
	
	if ( h == NULL )
	{
		err = MemErr;
		goto close_and_bail;
	}
	
	RsrcMapHandle rsrc_map = (RsrcMapHandle) h;
	
	rsrc_map_header& map = **rsrc_map;
	
	map.fork_header = header;
	map.next_map    = TopMapHndl;
	map.refnum      = refNum;
	
	type_list& types = *(type_list*) (*h + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	type_header* type = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = type->count_1;
		UInt16 offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			rsrc->handle = NULL;
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
	
	TopMapHndl = h;
	
	ResErr = noErr;
	
	return CurMap = refNum;
	
close_and_bail:
	FSClose( refNum );
	
bail:
	ResErr = err;
	return -1;
}

asm
pascal short OpenResFile_patch( const Byte* name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	MOVEQ.L  #0,D0    // vRefNum = 0
	
	JSR      OpenResFile_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
void CloseResFile_core( short refnum : __D0 )
{
	UpdateResFile_patch( refnum );
	
	RsrcMapHandle& rsrc_map = find_rsrc_map( refnum );
	
	if ( rsrc_map == NULL )
	{
		return;
	}
	
	rsrc_map_header& map = **rsrc_map;
	
	type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	type_header* type = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = type->count_1;
		UInt16 offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( Handle h = rsrc->handle )
			{
				master_pointer& mp = *(master_pointer*) h;
				
				mp.flags -= kHandleIsResourceMask;
				
				DisposeHandle( h );
				
				rsrc->handle = NULL;
			}
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
	
	Handle h = (Handle) rsrc_map;
	
	rsrc_map = (RsrcMapHandle) map.next_map;
	
	if ( CurMap == refnum )
	{
		CurMap = rsrc_map ? rsrc_map[0]->refnum : -1;
	}
	
	if ( ! (map.attrs & mapReadOnly)  &&  was_written( get_FCB( refnum ) ) )
	{
		// Write a clean resource map.
		
		map.next_map = NULL;
		map.refnum   = 0;
		
		OSErr err;
		
		const rsrc_fork_header& fork = map.fork_header;
		
		SInt32 offset_to_map = fork.offset_to_map;
		SInt32 length_of_map = fork.length_of_map;
		
		(err = SetFPos( refnum, fsFromStart, offset_to_map ))  ||
		(err = FSWrite( refnum, &length_of_map, &map ));
	}
	
	DisposeHandle( h );
	
	FSClose( refnum );
}

static inline
short map_refnum( Handle h )
{
	rsrc_map_header& map = **(RsrcMapHandle) h;
	
	return map.refnum;
}

static
void CloseResFile_handler( short refnum : __D0 )
{
	if ( refnum == CurApRefNum )
	{
		/*
			If we're asked to close the application's resource file,
			then close all resource files above it first.
		*/
		
		while ( (refnum = map_refnum( TopMapHndl )) != CurApRefNum )
		{
			CloseResFile_core( refnum );
		}
	}
	
	CloseResFile_core( refnum );
}

asm
pascal void CloseResFile_patch( short refnum )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D0
	
	JSR      CloseResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #2,SP
	
	JMP      (A0)
}

asm
pascal short OpenRFPerm_patch( const Byte* name, short vRefNum, SInt8 perm )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.B   (A2)+,D1  // payload in the high byte
	ADDQ.L   #1,A2     // skip the low byte
	MOVE.W   (A2)+,D0
	MOVEA.L  (A2)+,A0
	
	JSR      OpenResFile_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #8,SP
	
	JMP      (A0)
}
