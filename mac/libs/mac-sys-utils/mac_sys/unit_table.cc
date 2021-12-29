/*
	unit_table.cc
	-------------
*/

#include "mac_sys/unit_table.hh"

// Mac OS
#ifndef __APPLE__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif


namespace mac {
namespace sys {
	
	const unsigned char* get_driver_name( mac::types::AuxDCE** h )
	{
	#ifndef __APPLE__
		
		if ( h  &&  *h )
		{
			AuxDCE& dce = (AuxDCE&) **h;
			
			const bool ram_based = dce.dCtlFlags & dRAMBasedMask;
			
			void* drvr = dce.dCtlDriver;
			
			// Dereferences a handle if ram_based
			const DRVRHeaderPtr header = ram_based ? *(DRVRHeader **) drvr
			                                       :  (DRVRHeader * ) drvr;
			
			if ( header )
			{
				return header->drvrName;
			}
		}
		
	#endif
		
		return "\p";
	}
	
}
}
