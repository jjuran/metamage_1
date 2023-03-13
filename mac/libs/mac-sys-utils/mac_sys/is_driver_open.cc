/*
	is_driver_open.cc
	-----------------
*/

#include "mac_sys/is_driver_open.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifdef __MACOS__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif

// mac-glue-utils
#include "mac_glue/StringCompare.hh"

// mac-sys-utils
#include "mac_sys/unit_table.hh"


namespace mac {
namespace sys {

bool is_driver_open( const unsigned char* driver_name )
{
	const short count = get_unit_table_entry_count();
	
#if ! TARGET_API_MAC_CARBON
	
	for ( short unit = 0;  unit < count;  ++unit )
	{
		DCtlHandle dceHandle = GetDCtlEntry( -unit );
		
		if ( dceHandle != NULL  &&  dceHandle[0]->dCtlDriver != NULL )
		{
			const int flags = dceHandle[0]->dCtlFlags;
			const int inRAM = flags & dRAMBasedMask;
			
			DRVRHeader* drvr = inRAM ? *(DRVRHeader**) dceHandle[0]->dCtlDriver
			                         :  (DRVRHeader* ) dceHandle[0]->dCtlDriver;
			
			ConstStr255Param name = drvr->drvrName;
			
			if ( mac::glue::EqualString_sans_case( name, driver_name ) )
			{
				return flags & dOpenedMask;
			}
		}
	}
	
#endif
	
	return false;
}

}
}
