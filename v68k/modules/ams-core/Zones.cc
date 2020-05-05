/*
	Zones.cc
	--------
*/

#include "Zones.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// ams-common
#include "callouts.hh"

Ptr   HeapEnd  : 0x0114;
THz   TheZone  : 0x0118;
OSErr MemErr   : 0x0220;
THz   ApplZone : 0x02AA;
long  Lo3Bytes : 0x031A;


struct block_header
{
	union
	{
		UInt8   tag;
		UInt32  size;
	};
	
	union
	{
		THz     zone;
		UInt32  misc;
	};
};

static inline
block_header* get_block_header( Ptr logical_addr )
{
	return (block_header*) (logical_addr - sizeof (block_header));
}

static inline
Ptr data( block_header* block )
{
	return (Ptr) block + sizeof (block_header);
}

static inline
Size size( block_header* block )
{
	return block->size & Lo3Bytes;
}

static inline
block_header* next( block_header* block )
{
	return (block_header*) ((Ptr) block + size( block ));
}

static inline
block_header* begin( THz zone )
{
	return (block_header*) &zone->heapData;
}

static inline
block_header* end( THz zone )
{
	return (block_header*) zone->bkLim;
}

enum
{
	minimum_block_size        = 12,
	sizeof_zone_trailer_block = minimum_block_size,
};

#pragma mark -
#pragma mark Initialization and Allocation
#pragma mark -

void InitApplZone_patch()
{
	const Ptr zoneEnd = (Ptr) ApplZone + 1024 * 6;
	
	InitZone( NULL, 64, zoneEnd, ApplZone );
	
	TheZone = ApplZone;
	HeapEnd = ApplZone->bkLim;
}

struct InitZone_Params
{
	Ptr          start;
	Ptr          limit;
	short        moreMasters;
	GrowZoneUPP  growZone;
};

void InitZone_patch( InitZone_Params* params : __A0 )
{
	Ptr start = params->start;
	
	Zone& zone = *(THz) start;
	
	fast_memset( start, '\0', params->limit - start );
	
	Ptr heapData = (Ptr) &zone.heapData;
	Ptr data_end = params->limit - sizeof_zone_trailer_block;
	
	Size data_size = data_end - heapData;
	
	zone.bkLim    = data_end;
	zone.zcbFree  = data_size;  // number of bytes occupied by free blocks
	zone.gzProc   = params->growZone;
	zone.moreMast = params->moreMasters;
	
	block_header* single_block = begin( &zone );
	block_header* zone_trailer = end  ( &zone );
	
	single_block->size = data_size;
	zone_trailer->size = sizeof_zone_trailer_block;
}

short SetApplLimit_patch( Ptr p : __A0 )
{
	return MemErr = noErr;
}

void MaxApplZone_patch()
{
}

void MoreMasters_patch()
{
	MemErr = noErr;
}

#pragma mark -
#pragma mark Freeing Space in the Heap
#pragma mark -

long FreeMem_patch()
{
	return 1024 * 1024;
}

asm void MaxMem_patch()
{
	SUBA.L   A0,A0
	MOVE.L   #0x100000,D0
	RTS
}

long CompactMem_patch( long needed : __D0, short trap_word : __D1 )
{
	return needed;
}

short ReserveMem_patch( long needed : __D0, short trap_word : __D1 )
{
	return MemErr = noErr;
}

short PurgeMem_patch( long needed : __D0, short trap_word : __D1 )
{
	return MemErr = noErr;
}

#pragma mark -
#pragma mark Grow Zone Operations
#pragma mark -

short SetGrowZone_patch( void* proc : __A0 )
{
	return MemErr = noErr;
}

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

void MoveHHi_patch( Handle h : __A0 )
{
}
