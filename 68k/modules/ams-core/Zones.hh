/*
	Zones.hh
	--------
*/

#ifndef ZONES_HH
#define ZONES_HH

struct InitZone_Params;
struct Zone;

typedef Zone*   THz;
typedef char*   Ptr;
typedef char**  Handle;

typedef long (*FreeMem_ProcPtr)();

extern FreeMem_ProcPtr old_FreeMem;

Ptr zone_alloc_nonrel( THz zone, long logical_size );

bool zone_free_nonrel( Ptr logical_block_addr );

void InitApplZone_patch();

void InitZone_patch( InitZone_Params* params : __A0 );

short SetApplLimit_patch( Ptr p : __A0 );

void MaxApplZone_patch();

void MoreMasters_patch();

THz GetZone_patch();

short SetZone_patch( THz z : __A0 );

long FreeMem_patch( short trap_word : __D1 );

void MaxMem_patch( short trap_word : __D1 );

long CompactMem_patch( long needed : __D0, short trap_word : __D1 );

short ReserveMem_patch( long needed : __D0, short trap_word : __D1 );
short PurgeMem_patch  ( long needed : __D0, short trap_word : __D1 );

short SetGrowZone_patch( void* proc : __A0 );

void MoveHHi_patch( Handle h : __A0 );

long MaxBlock_patch( short trap_word : __D1 );

#endif
