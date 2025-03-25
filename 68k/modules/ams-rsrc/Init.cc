/*
	Init.cc
	-------
*/

#include "Init.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// ams-rsrc
#include "rsrc_fork.hh"


Ptr   MemTop      : 0x0108;
Ptr   ApplZone    : 0x02AA;
short CurApRefNum : 0x0900;
Str31 CurApName   : 0x0910;
Handle TopMapHndl : 0x0A50;
short SysMap      : 0x0A58;


typedef rsrc_map_header**  RsrcMapHandle;


static asm
void run_INIT( ProcPtr proc : __A0 )
{
	LINK     A6,#0
	MOVEM.L  D3-D7/A2-A4,-(SP)
	
	JSR      (A0)
	
	MOVEM.L  (SP)+,D3-D7/A2-A4
	UNLK     A6
	RTS
}

pascal short InitResources_patch()
{
	SysMap = OpenResFile( "\p" "AMS Resources" );
	
	short index = 0;
	
	while ( Handle h = GetIndResource( 'INIT', ++index ) )
	{
		ProcPtr proc = (ProcPtr) *h;
		
		run_INIT( proc );
	}
	
	return SysMap;
}

static inline
bool handle_in_ApplZone( Handle h )
{
	/*
		Compare addresses against MemTop to distinguish those in the ApplZone
		heap from those in persistent private memory above ROM.
		
		Don't use HeapEnd, because the heap has already been shrunk to its
		original size by the time we're called to clear out resources from
		the system resource file that could have been allocated higher in the
		application heap.
		
		Don't use ApplLimit, just in case an application sets it to a value
		less than HeapEnd (which is harmless, or at least mostly harmless).
	*/
	
	return h  &&  *h > ApplZone  &&  *h < MemTop;
	
	/*
		We're actually checking the data block address above.  We can't use
		the simpler logic below to check the master pointer address, because
		master pointer blocks are always allocated out of private memory.
	*/
	
	return (Ptr) h > ApplZone  &&  (Ptr) h < MemTop;
}

static
void release_ApplZone_resources()
{
	if ( MemTop == NULL )
	{
		return;
	}
	
	rsrc_map_header& map = **(RsrcMapHandle) TopMapHndl;
	
	type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	type_header* type = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = type->count_1;
		UInt16 offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( handle_in_ApplZone( rsrc->handle ) )
			{
				rsrc->handle = NULL;
			}
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
}

pascal void RsrcZoneInit_patch()
{
	release_ApplZone_resources();
	
	CurApRefNum = OpenResFile( CurApName );
}
