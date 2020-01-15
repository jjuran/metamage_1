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

// ams-common
#include "callouts.hh"

// ams-seg
#include "options.hh"


short CurApRefNum  : 0x0900;
Ptr   CurrentA5    : 0x0904;
Ptr   CurStackBase : 0x0908;
Str31 CurApName    : 0x0910;
short CurJTOffset  : 0x0934;

Handle AppParmHandle : 0x0AEC;


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
	
	if ( segnum == 1 )
	{
		enum { _FindControl = 0xA96C };
		
		uint16_t* p = (uint16_t*) &code[0][ 0x3afe ];
		
		if ( *p++ == _FindControl  &&  *p == 0x31DF )
		{
			/*
				Hot-patch Lode Runner to fix a crash.
				
				I'm not sure how this happened; it's almost certainly not a
				compiler bug, seeing how the correct instruction also occurs
				later in the same function, but it seems an odd error to make
				when writing assembly language by hand.
				
				The word following the opcode is the same, though in d68k it
				disassembles differently: (int16_t) 0xae28 == -20952
				The error is that the opcode's destination effective address
				is wrong in the shipped code.  The reason it "works" at all is
				because 68000-based Macs lack an MMU, memory wraps around, and
				writes never fail, so $ae28 (which is actually $ffffae28) maps
				to $0001ae28 on an original Macintosh, which is in the 29th
				scanline of screen memory (and remains so on later Macs with
				512x342 screens (with more memory) because the screen memory
				is in the same location relative to the /high end/ of memory).
				
				But xv68k doesn't (currently) map the screen that way, so
				clicking Lode Runner's game window writes to unmapped memory
				and crashes.  To avoid that, hot-patch the broken instruction
				to what it should have been.
				
				Old:  MOVE.W   (A7)+,0xae28       // dst: mode 7, reg 0
				New:  MOVE.W   (A7)+,(-20952,A5)  // dst: mode 5, reg 5
			*/
			
			*p = 0x3B5F;
		}
	}
	
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
	const unsigned char* appName = (StringPtr) pb->reserved1;
	
	if ( appName == NULL )
	{
		DebugStr( "\p" "LaunchApplication() not supported" );
		
		ExitToShell_patch();
	}
	
	const uint8_t len = appName[ 0 ];
	
	if ( len > 31 )
	{
		DebugStr( "\p" "Application name longer than 31 bytes" );
		
		ExitToShell_patch();
	}
	
	fast_memcpy( CurApName, appName, 1 + len );
	
	RsrcZoneInit();
	
	Handle code0 = GetResource( 'CODE', 0 );
	
	if ( code0 == NULL )
	{
		SysError( 26 );
	}
	
	const jump_table_header& header = *(jump_table_header*) *code0;
	
	CurJTOffset = header.jmptable_offset;
	
	const uint32_t total_a5_size = header.above_a5_size
	                             + header.below_a5_size;
	
	const uint32_t stack_size = 64 * 1024;
	
	const uint32_t total_alloc_size = stack_size + total_a5_size;
	
	Ptr alloc = NewPtr( total_alloc_size );
	
	CurStackBase = alloc + stack_size;
	
	CurrentA5 = CurStackBase + header.below_a5_size;
	
	void* jump_table = CurrentA5 + CurJTOffset;
	
	fast_memcpy( jump_table, &header + 1, header.jmptable_size );
	
	ReleaseResource( code0 );
	
	AppParmHandle = NewHandleClear( 4 );
	
	above_A5_area* above_A5 = (above_A5_area*) CurrentA5;
	above_A5->Finder_handle = AppParmHandle;
	
	void* start = (char*) jump_table + 2;
	
	asm
	{
		MOVEA.L  CurrentA5,A5
		MOVEA.L  start,A0
		MOVE.L   CurStackBase,SP
		JSR      (A0)
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
