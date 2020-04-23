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
Ptr   ApplLimit: 0x0130;
OSErr MemErr   : 0x0220;
THz   ApplZone : 0x02AA;
long  Lo3Bytes : 0x031A;


enum tag_byte
{
	Tag_free   = 0x00,  // free space, zone trailer
	Tag_nonrel = 0x40,  // NewPtr, MoreMasters
	Tag_rel    = 0x80,  // NewHandle
	Tag_unused = 0xC0,  // unused block type
	
	Tag_type_mask = 0xC0,  // block type field
	Tag_zero_mask = 0x30,  // reserved bits, should be zero
	Tag_size_mask = 0x0F,  // size correction, a.k.a. padding
};

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

static inline
Size padded( Size size )
{
	return size == 0 ? 4 : size + 3 & ~3;
}

static
block_header* locate_free_block( THz zone, Size minimum )
{
	block_header* p = begin( zone );
	block_header* q = NULL;
	
	block_header* trailer = end( zone );
	
	while ( p < trailer )
	{
		if ( p->tag == Tag_free )
		{
			if ( q != NULL )
			{
				q->size += p->size;  // Merge the two free blocks
				
				if ( q->size >= minimum )
				{
					return q;
				}
			}
			else
			{
				q = p;
			}
			
			if ( p->size >= minimum )
			{
				return p;
			}
		}
		else
		{
			q = NULL;
		}
		
		p = next( p );
	}
	
	return NULL;
}

static
block_header* zone_alloc( THz zone, long logical_size )
{
	const Size minimum_size = sizeof (block_header) + padded( logical_size );
	
	block_header* free_block = locate_free_block( zone, minimum_size );
	
	if ( free_block )
	{
		Size delta = free_block->size - minimum_size;
		
		if ( delta >= minimum_block_size )
		{
			free_block->size = minimum_size;
			
			next( free_block )->size = delta;
		}
		
		const Size physical_size = free_block->size;
		
		zone->zcbFree -= physical_size;
		
		// Store the padding in the tag byte.  Caller will set block type.
		free_block->tag = physical_size - sizeof (block_header) - logical_size;
	}
	
	return free_block;
}

Ptr zone_alloc_nonrel( THz zone, long logical_size )
{
	block_header* block = zone_alloc( zone, logical_size );
	
	if ( block == NULL )
	{
		return NULL;
	}
	
	block->tag |= Tag_nonrel;
	block->zone = zone;
	
	return data( block );
}

static inline
void zone_free( THz zone, block_header* block )
{
	block->tag = 0;  // Mark as a free block
	
	zone->zcbFree += block->size;
}

bool zone_free_nonrel( Ptr logical_block_addr )
{
	block_header* block = get_block_header( logical_block_addr );
	
	if ( THz zone = block->zone )
	{
		zone_free( zone, block );
	}
	
	return block->zone != NULL;
}

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
	if ( ApplLimit >= HeapEnd + minimum_block_size )
	{
		const UInt32 delta = ApplLimit - HeapEnd;
		
		block_header* old_trailer = (block_header*) HeapEnd;
		block_header* new_trailer = (block_header*) ApplLimit;
		
		HeapEnd = ApplLimit;
		
		ApplZone->bkLim = ApplLimit;
		ApplZone->zcbFree += delta;
		
		old_trailer->size = delta;  // physical size of new free block
		new_trailer->size = sizeof_zone_trailer_block;
	}
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
