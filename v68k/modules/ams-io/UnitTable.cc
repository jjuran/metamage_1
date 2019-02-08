/*
	UnitTable.cc
	------------
*/

#include "UnitTable.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// iota
#include "iota/char_types.hh"


DCtlHandle*  UTableBase  : 0x011C;
short        UnitNtryCnt : 0x01D2;

short MemErr : 0x0220;


DRVRHeader* get_driver( const DCtlEntry* dce )
{
	void* ptr;
	
	if ( dce  &&  (ptr = dce->dCtlDriver) )
	{
		if ( ! (dce->dCtlFlags & dRAMBasedMask)  ||  (ptr = *(Handle) ptr) )
		{
			return (DRVRHeader*) ptr;
		}
	}
	
	return NULL;
}

static
bool equivalent_fsnames( const UInt8* a, const UInt8* b, UInt16 len )
{
	while ( len-- > 0 )
	{
		if ( iota::to_lower( *a++ ) != iota::to_lower( *b++ ) )
		{
			return false;
		}
	}
	
	return true;
}

static
bool equivalent_fsnames( ConstStr255Param a, ConstStr255Param b )
{
	return *a == *b  &&  equivalent_fsnames( a + 1, b + 1, *a );
}

short find_unit_entry_by_name( ConstStr255Param name )
{
	for ( short i = 0;  i < UnitNtryCnt;  ++i )
	{
		if ( DCtlHandle h = UTableBase[ i ] )
		{
			if ( DRVRHeader* drvr = get_driver( *h ) )
			{
				if ( equivalent_fsnames( drvr->drvrName, name ) )
				{
					return i;
				}
			}
		}
	}
	
	return -1;
}

static
short find_unused_unit_entry()
{
	/*
		Leave some space for the standard drivers and DAs.  Many applications
		will, for example, write to refnum -4 and assume that it's .Sound and
		that it's open (and this is documented to be the case).
		
		At the very least, leave unit number zero vacant.  An application
		might take the liberty of installing a driver in UTableBase[ 0 ],
		in which case find_unit_entry_by_name() will match it, but under no
		circumstance will we return it as an unused entry.  Therefore, we can
		use zero as a failure code.
	*/
	
	for ( short i = 20;  i < UnitNtryCnt;  ++i )
	{
		if ( UTableBase[ i ] == NULL )
		{
			return i;
		}
	}
	
	return 0;
}

short install( DRVRHeader** d, short i )
{
	DCtlHandle h = (DCtlHandle) NewHandleClear( sizeof (DCtlEntry) );
	
	if ( h == NULL )
	{
		return MemErr;
	}
	
	DRVRHeader& drvr = **d;
	
	DCtlEntry& dce = **h;
	
	dce.dCtlDriver = (Ptr) d;
	dce.dCtlFlags  = drvr.drvrFlags | dRAMBasedMask;
	
	dce.dCtlRefNum = ~i;
	
	dce.dCtlDelay  = drvr.drvrDelay;
	dce.dCtlEMask  = drvr.drvrEMask;
	dce.dCtlMenu   = drvr.drvrMenu;
	
	UTableBase[ i ] = h;
	
	return i;
}

short install( DRVRHeader** d )
{
	if ( const short i = find_unused_unit_entry() )
	{
		return install( d, i );
	}
	
	return 0;
}
