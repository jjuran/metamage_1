/*
	Zones.hh
	--------
*/

#ifndef ZONES_HH
#define ZONES_HH

typedef char*   Ptr;
typedef char**  Handle;

short SetApplLimit_patch( Ptr p : __A0 );

void MaxApplZone_patch();

void MoreMasters_patch();

long FreeMem_patch();

void MaxMem_patch();

long CompactMem_patch( long needed : __D0, short trap_word : __D1 );

short ReserveMem_patch( long needed : __D0, short trap_word : __D1 );
short PurgeMem_patch  ( long needed : __D0, short trap_word : __D1 );

short SetGrowZone_patch( void* proc : __A0 );

void MoveHHi_patch( Handle h : __A0 );

#endif
