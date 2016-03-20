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
#include <stdlib.h>
#include <string.h>

// macos
#include "options.hh"


Str31 CurApName   : 0x0910;
short CurJTOffset : 0x0934;


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

static inline
asm jump_table_entry* get_jump_table_offset( uint16_t offset : __D0 ) : __A0
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
		ExitToShell_patch();
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

pascal short Launch_patch( LaunchParamBlockRec* pb : __A0 ) : __D0
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
	
	memcpy( CurApName, appName, 1 + len );
	
	Handle code0 = GetResource( 'CODE', 0 );
	
	const jump_table_header header = *(jump_table_header*) *code0;
	
	CurJTOffset = header.jmptable_offset;
	
	const uint32_t total_a5_size = header.above_a5_size
	                             + header.below_a5_size;
	
	void* alloc = malloc( total_a5_size );
	
	void* new_a5 = (char*) alloc + header.below_a5_size;
	
	void* jump_table = (char*) new_a5 + CurJTOffset;
	
	memcpy( jump_table, (jump_table_header*) *code0 + 1, header.jmptable_size );
	
	ReleaseResource( code0 );
	
	void* start = (char*) jump_table + 2;
	
	asm
	{
		MOVEA.L  new_a5,A5
		MOVEA.L  start,A0
		JMP      (A0)
	}
	
	return 0;  // not reached
}

pascal void ExitToShell_patch()
{
	if ( linger_on_exit )
	{
		pause();
	}
	
	_exit( 0 );
}
