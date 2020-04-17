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
	
	Ptr heapData = start + 52;
	Ptr data_end = params->limit - 12;
	
	Size data_size = data_end - heapData;
	
	*(UInt32*) heapData = data_size;  // free block physical size
	
	zone.bkLim    = data_end;
	zone.zcbFree  = data_size;  // number of bytes occupied by free blocks
	zone.gzProc   = params->growZone;
	zone.moreMast = params->moreMasters;
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
