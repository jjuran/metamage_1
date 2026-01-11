/*
	Segments.cc
	-----------
*/

#include "Segments.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SHUTDOWN__
#include <ShutDown.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// log-of-war
#include "logofwar/print.hh"
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"


#pragma exceptions off


Ptr   MemTop       : 0x0108;
Ptr   BufPtr       : 0x010C;
Ptr   ApplLimit    : 0x0130;
Str15 FinderName   : 0x02E0;
long  DefltStack   : 0x0322;
Ptr   ScrnBase     : 0x0824;
short CurApRefNum  : 0x0900;
Ptr   CurrentA5    : 0x0904;
Ptr   CurStackBase : 0x0908;
Str31 CurApName    : 0x0910;
short CurJTOffset  : 0x0934;
short CurPageOption : 0x0936;

Handle AppParmHandle : 0x0AEC;


using logofwar::hex32_t;

pascal void GetAppParms_patch( StringPtr name, short* refNum, Handle* hp )
{
	fast_memcpy( name, CurApName, 1 + CurApName[ 0 ] );
	
	*refNum = CurApRefNum;
	*hp     = AppParmHandle;
}

struct jump_table_header
{
	UInt32 above_a5_size;
	UInt32 below_a5_size;
	UInt32 jmptable_size;
	UInt32 jmptable_offset;
};

struct jump_table_entry_unloaded
{
	UInt16 offset;
	UInt16 opcode;
	UInt16 segnum;
	UInt16 trap;
};

struct jump_table_entry_loaded
{
	UInt16 segnum;
	UInt16 opcode;
	UInt32 addr;
};

union jump_table_entry
{
	jump_table_entry_loaded    loaded;
	jump_table_entry_unloaded  unloaded;
};

struct segment_header
{
	UInt16 table_offset;
	UInt16 entry_count;
};

struct LaunchParamBlockRec
{
	Byte*   reserved1;
	UInt16  reserved2;
	// ...
};

typedef struct GrafPort* GrafPtr;

struct above_A5_area
{
	GrafPtr*  thePort_addr;
	UInt32    unknown_4;
	void*     stdin_hook;
	void*     stdout_hook;
	Handle    Finder_handle;
	UInt32    unknown_20;
	UInt32    unknown_24;
	UInt32    unknown_28;
};

static inline asm
jump_table_entry* get_jump_table_offset( UInt16 offset : __D0 )
{
	MOVEA.L  A5,A0
	ADDA.W   CurJTOffset,A0
	ADDA.W   D0,A0
}

const UInt16 push_opcode = 0x3F3C;  // MOVE.W   #n,-(SP)
const UInt16 jump_opcode = 0x4EF9;  // JMP      0xABCD1234

static
void LoadSegment( short segnum : __D0 )
{
	using mac::glue::GetHandleSize_raw;
	
	Handle code = GetResource( 'CODE', segnum );
	
	if ( code == NULL )
	{
		SysError( 15 );
	}
	
	long addr = (long) *code;
	
	NOTICE = hex32_t( addr ), " -> ",
	         hex32_t( addr + GetHandleSize_raw( code ) ), ": 'CODE' ", segnum;
	
	const segment_header* segment = (segment_header*) *code;
	
	UInt16 offset = segment->table_offset;
	UInt16 count  = segment->entry_count;
	
	const void* start = segment + 1;  // skip header
	
	jump_table_entry* it = get_jump_table_offset( offset );
	
	do
	{
		jump_table_entry_unloaded&  unloaded = it->unloaded;
		jump_table_entry_loaded&    loaded   = it->loaded;
		
		const UInt16 offset = unloaded.offset;
		
		loaded.segnum = segnum;
		loaded.opcode = jump_opcode;
		loaded.addr   = (UInt32) start + offset;
		
		++it;
	}
	while ( --count != 0 );
}

pascal asm void LoadSeg_patch( short segnum )
{
	LINK     A6,#0
	MOVEM.L  D0-D2/A0-A1,-(SP)
	
	MOVE.W   8(A6),D0
	JSR      LoadSegment
	
	MOVEM.L  (SP)+,D0-D2/A0-A1
	UNLK     A6
	
	MOVE.L   (SP),2(SP)
	ADDQ.L   #2,SP
	
	SUBQ.L   #6,(SP)
	RTS
}

pascal void UnloadSeg_patch( void* ptr )
{
}

short Launch_patch( LaunchParamBlockRec* pb : __A0 )
{
	/*
		Chapter 10 of Scott Knaster's book /How to Write Macintosh Software/,
		"Operating System Techniques", includes a "Play by Play" of how Launch
		works, in 24 numbered steps, paraphrased below.
	*/
	
	const Byte* appName = (StringPtr) pb->reserved1;
	
	if ( appName == NULL )
	{
		DebugStr( "\p" "LaunchApplication() not supported" );
		
		ExitToShell_patch();
	}
	
	NOTICE = "Launching application \"", CSTR( appName ), "\"";
	
	const Byte len = appName[ 0 ];
	
	if ( len > 31 )
	{
		DebugStr( "\p" "Application name longer than 31 bytes" );
		
		ExitToShell_patch();
	}
	
	// 1. Set CurApName and CurPageOption.
	
	fast_memcpy( CurApName, appName, 1 + len );
	
	CurPageOption = pb->reserved2;
	
	Ptr stack_bottom = CurPageOption < 0 ? ScrnBase - 0x8000
	                 : CurPageOption > 0 ? ScrnBase - 0x0600
	                 :                     ScrnBase;
	
	BufPtr = stack_bottom;
	
	if ( CurPageOption < 0 )
	{
		WARNING = "Allocating page 2 buffers";
	}
	else if ( CurPageOption > 0 )
	{
		WARNING = "Allocating page 2 audio buffer";
	}
	
	// 2. Send goodBye to open drivers with dNeedGoodBye set.
	
	// 3. Close previous application's resource file.
	
	if ( CurApRefNum > 0 )
	{
		CloseResFile( CurApRefNum );
		
		CurApRefNum = 0;
	}
	
	// 4. Reset SP to BufPtr.
	// 5. Disable the stack sniffer.
	// 6. Save any scrap onto the stack.
	
	// 7. Reinitialize the application heap.
	
	if ( MemTop )
	{
		InitApplZone();
	}
	
	// 8. Move the scrap into a new handle (if any).
	// 9. Reinitialize the unit table.
	// 10. Clear system globals.
	
	// 11. Open the application's resfile, stored in CurApRefNum...
	RsrcZoneInit();
	
	// 12. ... or raise System Error 26.
	
	if ( CurApRefNum < 0 )
	{
		SysError( 26 );
	}
	
	// 13. Load 'CODE' 0 or raise System Error 26.
	
	Handle code0 = GetResource( 'CODE', 0 );
	
	if ( code0 == NULL )
	{
		SysError( 26 );
	}
	
	// 14. Expand the stack to include any requested page 2 buffers.
	// 15. Allocate the area above A5 on the stack.
	
	const jump_table_header& header = *(jump_table_header*) *code0;
	
	CurJTOffset = header.jmptable_offset;
	
	const UInt32 total_a5_size = header.above_a5_size
	                           + header.below_a5_size;
	
	const UInt32 stack_size = DefltStack;
	
	const UInt32 total_alloc_size = stack_size + total_a5_size;
	
	Ptr alloc = MemTop ? stack_bottom - total_alloc_size
	                   : NewPtrClear( total_alloc_size );
	
	// 'ss' == 0x7373, an odd address and an invalid instruction.
	
	fast_memset( alloc, 's', stack_size );
	
	// 16. Set CurrentA5.
	// 17. Set CurStackBase.
	
	CurStackBase = alloc + stack_size;
	
	CurrentA5 = CurStackBase + header.below_a5_size;
	
	// 18. Enable the stack sniffer.
	
	// 19. Place the jump table above A5.
	
	void* jump_table = CurrentA5 + CurJTOffset;
	
	fast_memcpy( jump_table, &header + 1, header.jmptable_size );
	
	// 20. Release 'CODE' 0.
	ReleaseResource( code0 );
	
	// 21. Place the Finder information handle above A5.
	above_A5_area* above_A5 = (above_A5_area*) CurrentA5;
	above_A5->Finder_handle = AppParmHandle;
	
	// 22. Set the application heap limit.
	
	if ( MemTop )
	{
		ApplLimit = alloc;
	}
	
	void* start = (char*) jump_table + 2;
	
	asm
	{
		MOVEA.L  CurrentA5,A5
		MOVEA.L  start,A3
		MOVE.L   CurStackBase,SP
		
	// 23. Clear D7.
		MOVEQ.L  #0,D7
		
	// 24. Start the application.
		JSR      (A3)
		DC.W     0xA9F4  // _ExitToShell
	}
	
	return 0;  // not reached
}

struct Loader_AppParams
{
	short message;
	short count;
	
	short   vRefNum;
	OSType  type;
	short   versNum;
	Str31   name;
};

void ExitToShell_patch()
{
	if ( FinderName[ 0 ] )
	{
		SetHandleSize( AppParmHandle, sizeof (Loader_AppParams) );
		
		Loader_AppParams& params = *(Loader_AppParams*) *AppParmHandle;
		
		fast_memset( &params, '\0', sizeof params - sizeof (Str31) );
		fast_memcpy( params.name,   CurApName,      sizeof (Str31) );
		
		params.count = 1;
		params.type = 'APPL';
		
		LaunchParamBlockRec pb;
		
		pb.reserved1 = FinderName;
		pb.reserved2 = 0;
		
		Launch_patch( &pb );
	}
	
	ShutDwnPower();
}
