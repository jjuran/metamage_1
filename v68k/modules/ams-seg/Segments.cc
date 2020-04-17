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

// Standard C
#include <string.h>

// ams-common
#include "callouts.hh"

// ams-seg
#include "options.hh"


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

static const uint16_t Lemmings_wait_loop[] =
{
	0x42A7,  // CLR.L    -(A7)
	0xA975,  // _TickCount
	0xBE9F,  // CMP.L    (A7)+,D7
	0x64F8,  // BCC.S    *-6
};

static const uint16_t Lemmings_wait_trap[] =
{
	0x5288,  // ADDQ.L   #1,A0
	0x91F8,  // SUBA.L   0x016A,A0
	0x016A,  //          ^^^^^^
	0xA23B,  // _DelayEdge (_Delay | kDelayTickEdgeMask)
};

static
void apply_hotpatches( Handle code, short segnum )
{
	enum
	{
		long_pop_to_absolute_short = 0x31DF,  // dst: mode 7, reg 0
		long_pop_to_displaced_A5   = 0x3B5F,  // dst: mode 5, reg 5
		byte_clear_to_displaced_A5 = 0x422D,  // dst: mode 5, reg 5
		byte_set_T_to_displaced_A5 = 0x50ED,  // dst: mode 5, reg 5
		
		_FindControl = 0xA96C,
	};
	
	Size size = GetHandleSize( code );
	
	if ( segnum == 1 )
	{
		if ( size < 0x3b02 )
		{
			return;
		}
		
		uint16_t* p = (uint16_t*) &code[0][ 0x3afe ];
		
		if ( *p++ == _FindControl  &&  *p == long_pop_to_absolute_short )
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
				
				Old:  MOVE.W   (A7)+,0xae28
				New:  MOVE.W   (A7)+,(-20952,A5)
			*/
			
			*p = long_pop_to_displaced_A5;
		}
	}
	
	if ( segnum == 2 )
	{
		if ( size < 0x537a )
		{
			return;
		}
		
		uint16_t* p = (uint16_t*) &code[0][ 0x537a ];
		
		if ( *--p == (uint16_t) -342  &&  *--p == byte_clear_to_displaced_A5 )
		{
			/*
				Hot-patch Lemmings to fix a crash that occurs right at the end
				of a level played with sound -- /before/ you're given the code
				to the next level!
				
				In CODE segment 2, at offset $00536c, there's a function that
				stops the sound engine and frees various resources involved,
				one of which is an array of 104-byte elements.  Another array
				has 38-byte elements.  There's a VBL task struct from (-310,A5)
				to (-296,A5); anything at an address just higher than that can
				be considered VBL payload data, including a flag, the element
				count, and the first array's base address.
				
				Here's what the function does:
				  * sets a flag at (-296,A5)
				  * clears a flag at (-342,A5)
				  * tests an element count at (-294,A5), bailing out if zero
				  * calls a function that marks elements invalid
				  * removes a VBL task via _VRemove
				  * loops over the array, calling a no-op on each element
				  * tests the base address of the array it already looped over
				  * frees the array via _DisposePtr
				  * clears the array pointer variable at (-292,A5)
				  * calls the function at $0051d8
				
				Here's what that last function does:
				  * tests another array base address at (-350,A5)
				  * loops over the that array, calling $00510c on each element
				  * frees the array via _DisposePtr
				
				Here's what /that/ function does:
				  * calls $005960
				  * (other stuff that's not relevant)
				
				And finally, here's what **that** function does:
				  * tests the flag at (-342,A5), bailing out if set
				  * loops over the array at (-292,A5), testing each first byte
				
				The VBL task is removed, the first array is destroyed, and the
				application crashes processing the first element of the second
				array, because it dereferences the first byte of the first slot
				of the first array, whose pointer location was already cleared,
				resulting in a NULL dereference.
				
				It's not entirely clear what the exact semantics of the flags
				are, but they appear to be similar.  The VBL payload flag at
				(-296,A5) is invariably cleared on VBL install and set on VBL
				removal -- so it has negative polarity.  It seems to mean 'VBL
				not installed'.  The other flag, at (-342,A5), is set early in
				an initialization routine and reset in the same function after
				VInstall() succeeds.  Most of the subroutines that test it bail
				out if it's set, including one that then sets it and removes
				the VBL.  The one exception bails out if it's clear and then
				immediately clears it, subsequently initializing the first
				array above and installing the VBL.
				
				It's another reversed polarity flag, by all indications except
				one:  The function at $00536c, which calls VRemove(), /clears/
				rather than sets the flag.  If we patch that function to set
				the flag instead, the function at $005960 -- that crashes when
				it accesses NULL -- will bail out instead.  We can't set it to
				$01 in just the two words we have to play with, but we can set
				it to $FF, which is just as good as far as TST is concerned.
				
				Old:  CLR.B    (-342,A5)
				New:  ST.B     (-342,A5)
			*/
			
			*p = byte_set_T_to_displaced_A5;
		}
		
		p = (uint16_t*) &code[0][ 0x2aaa ];
		
		if ( memcmp( p, Lemmings_wait_loop, sizeof Lemmings_wait_loop ) == 0 )
		{
			fast_memcpy( p, Lemmings_wait_trap, sizeof Lemmings_wait_trap );
		}
	}
}

static
void LoadSegment( short segnum : __D0 )
{
	Handle code = GetResource( 'CODE', segnum );
	
	if ( code == NULL )
	{
		SysError( 15 );
	}
	
	apply_hotpatches( code, segnum );
	
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
	
	CurPageOption = pb->reserved2;
	
	Ptr stack_bottom = CurPageOption < 0 ? ScrnBase - 0x8000
	                 : CurPageOption > 0 ? ScrnBase - 0x0600
	                 :                     ScrnBase;
	if ( MemTop )
	{
		InitApplZone();
	}
	
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
	
	const uint32_t stack_size = DefltStack;
	
	const uint32_t total_alloc_size = stack_size + total_a5_size;
	
	Ptr alloc = MemTop ? stack_bottom - total_alloc_size
	                   : NewPtr( total_alloc_size );
	
	CurStackBase = alloc + stack_size;
	
	CurrentA5 = CurStackBase + header.below_a5_size;
	
	void* jump_table = CurrentA5 + CurJTOffset;
	
	fast_memcpy( jump_table, &header + 1, header.jmptable_size );
	
	ReleaseResource( code0 );
	
	AppParmHandle = NewHandleClear( 4 );
	
	above_A5_area* above_A5 = (above_A5_area*) CurrentA5;
	above_A5->Finder_handle = AppParmHandle;
	
	if ( MemTop )
	{
		ApplLimit = alloc;
	}
	
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
