/*
	Zones.cc
	--------
*/

#include "Zones.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


OSErr MemErr  : 0x0220;


#pragma mark -
#pragma mark Initialization and Allocation
#pragma mark -

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
