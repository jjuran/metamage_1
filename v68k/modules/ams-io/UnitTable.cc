/*
	UnitTable.cc
	------------
*/

#include "UnitTable.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif


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
