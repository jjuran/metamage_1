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
//#include <LowMem.h>
#endif
#endif


namespace mac {
namespace sys {
	
	/*
		LMGetUnitTableEntryCount() has been observed to have a NULL T-Vector
		address in System 7.5.2 on a Pippin.  On the other hand, direct access
		to the low memory global UnitNtryCnt (as well as UTableBase) works
		just fine, even through Mac OS 9.2.2 (both booting natively and in
		Classic on Mac OS X).  We could check for the NULL function address
		before calling it, but there's no reason to do so -- just read the low
		memory locations.
		
		Special thanks to Keith Kaisershot for helping me test this.
		
		TODO:  In Carbon, try to load these symbols from InterfaceLib.
	*/
	
	mac::types::AuxDCE*** get_unit_table_base()
	{
	#if !TARGET_API_MAC_CARBON
		
//		return (mac::types::AuxDCE***) ::LMGetUTableBase();
		
		return *(mac::types::AuxDCE****) 0x011C;  // UTableBase
		
	#endif
		
		return 0;  // NULL
	}
	
	short get_unit_table_entry_count()
	{
	#if !TARGET_API_MAC_CARBON
		
//		return ::LMGetUnitTableEntryCount();
		
		return *(short*) 0x01D2;  // UnitNtryCnt
		
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
