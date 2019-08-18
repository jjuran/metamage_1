/*
	SystemVersion.cc
	----------------
*/

#include "MacFeatures/SystemVersion.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace MacFeatures
{
	
	unsigned SystemVersion()
	{
		enum { gestaltSystemVersion = 'sysv' };
		
		// FIXME:  This returns zero if _Gestalt is unimplemented.
		
		long result = 0;
		
		result = mac::sys::gestalt( gestaltSystemVersion );
		
		return result;
	}
	
}
