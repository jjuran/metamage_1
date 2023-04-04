/*
	Segments.cc
	-----------
*/

#include "Segments.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"

// ams-seg
#include "options.hh"


#pragma exceptions off


Ptr   MemTop       : 0x0108;
Ptr   ApplLimit    : 0x0130;
long  DefltStack   : 0x0322;
Ptr   ScrnBase     : 0x0824;
short CurApRefNum  : 0x0900;
Ptr   CurrentA5    : 0x0904;
Ptr   CurStackBase : 0x0908;
Str31 CurApName    : 0x0910;
short CurJTOffset  : 0x0934;
short CurPageOption : 0x0936;

Handle AppParmHandle : 0x0AEC;


enum hex32
{
	hex32_min = 0,
	hex32_max = 0xffffffff,
};

inline
void print( hex32 x )
{
	logofwar::print( "$" );
	
	logofwar::print_hex( x );
}

pascal void GetAppParms_patch( StringPtr name, short* refNum, Handle* hp )
{
	fast_memcpy( name, CurApName, 1 + CurApName[ 0 ] );
	
	*refNum = CurApRefNum;
	*hp     = AppParmHandle;
}

struct jump_table_header
{
	uint32_t above_a5_size;
	uint32_t below_a5_size;
	uint32_t jmptable_size;
	uint32_t jmptable_offset;
};

struct jump_table_entry_unloaded
{
	uint16_t offset;
	uint16_t opcode;
	uint16_t segnum;
	uint16_t trap;
};

struct jump_table_entry_loaded
{
	uint16_t segnum;
	uint16_t opcode;
	uint32_t addr;
};

union jump_table_entry
{
	jump_table_entry_loaded    loaded;
	jump_table_entry_unloaded  unloaded;
};

struct segment_header
{
	uint16_t table_offset;
	uint16_t entry_count;
};

struct LaunchParamBlockRec
{
	uint32_t reserved1;
	uint16_t reserved2;
	// ...
};

typedef struct GrafPort* GrafPtr;

struct above_A5_area
{
	GrafPtr*  thePort_addr;
	uint32_t  unknown_4;
	void*     stdin_hook;
	void*     stdout_hook;
	Handle    Finder_handle;
	uint32_t  unknown_20;
	uint32_t  unknown_24;
	uint32_t  unknown_28;
};

static inline asm
jump_table_entry* get_jump_table_offset( uint16_t offset : __D0 )
{
	MOVEA.L  A5,A0
	ADDA.W   CurJTOffset,A0
	ADDA.W   D0,A0
}

const uint16_t push_opcode = 0x3F3C;  // MOVE.W   #n,-(SP)
const uint16_t jump_opcode = 0x4EF9;  // JMP      0xABCD1234

static
void LoadSegment( short segnum : __D0 )
{
	Handle code = GetResource( 'CODE', segnum );
	
	if ( code == NULL )
	{
		SysError( 15 );
	}
	
	Size size = GetHandleSize( code );
	
	long addr = (long) *code;
	
	NOTICE = hex32( addr ), " -> ", hex32( addr + size ), ": 'CODE' ", segnum;
	
	const segment_header* segment = (segment_header*) *code;
	
	uint16_t offset = segment->table_offset;
	uint16_t count  = segment->entry_count;
	
	const void* start = segment + 1;  // skip header
	
	jump_table_entry* it = get_jump_table_offset( offset );
	
	do
	{
		jump_table_entry_unloaded&  unloaded = it->unloaded;
		jump_table_entry_loaded&    loaded   = it->loaded;
		
		const uint16_t offset = unloaded.offset;
		
		loaded.segnum = segnum;
		loaded.opcode = jump_opcode;
		loaded.addr   = (uint32_t) start + offset;
		
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
	
	const unsigned char* appName = (StringPtr) pb->reserved1;
	
	if ( appName == NULL )
	{
		DebugStr( "\p" "LaunchApplication() not supported" );
		
		ExitToShell_patch();
	}
	
	NOTICE = "Launching application \"", CSTR( appName ), "\"";
	
	const uint8_t len = appName[ 0 ];
	
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
	
	const uint32_t total_a5_size = header.above_a5_size
	                             + header.below_a5_size;
	
	const uint32_t stack_size = DefltStack;
	
	const uint32_t total_alloc_size = stack_size + total_a5_size;
	
	Ptr alloc = MemTop ? stack_bottom - total_alloc_size
	                   : NewPtr( total_alloc_size );
	
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

void ExitToShell_patch()
{
	if ( linger_on_exit )
	{
		pause();
	}
	
	_exit( 0 );
}
