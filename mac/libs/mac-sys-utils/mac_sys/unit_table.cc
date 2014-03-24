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
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#endif


namespace mac {
namespace sys {
	
	// FIXME:  Load these from InterfaceLib for Carbon
	
	mac::types::AuxDCE*** get_unit_table_base()
	{
	#if !TARGET_API_MAC_CARBON
		
		return (mac::types::AuxDCE***) ::LMGetUTableBase();
		
	#endif
		
		return 0;  // NULL
	}
	
	short get_unit_table_entry_count()
	{
	#if !TARGET_API_MAC_CARBON
		
		return ::LMGetUnitTableEntryCount();
		
	#endif
		
		return 0;
	}
	
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

