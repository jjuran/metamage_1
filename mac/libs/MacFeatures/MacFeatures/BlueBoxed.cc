/*
	BlueBoxed.cc
	------------
*/

#include "MacFeatures/BlueBoxed.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace MacFeatures
{
	
#ifndef __MACH__
	
	bool Is_BlueBoxed()
	{
		enum
		{
			gestaltMacOSCompatibilityBoxAttr    = 'bbox',
			gestaltMacOSCompatibilityBoxPresent = 0,
		};
		
		return mac::sys::gestalt_bit_set( gestaltMacOSCompatibilityBoxAttr,
		                                  gestaltMacOSCompatibilityBoxPresent );
	}
	
#endif
	
}
