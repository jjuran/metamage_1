/*
	ADB.cc
	------
*/

#include "MacFeatures/ADB.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace MacFeatures
{
	
#if TARGET_CPU_68K  &&  !__MC68020__
	
	bool Has_ADB()
	{
		enum { gestaltROMSize = 'rom ' };
		
		// FIXME:  This always returns false if _Gestalt is unimplemented.
		
		long result;
		
		result = mac::sys::gestalt( gestaltROMSize );
		
		// 256K ROM has ADB
		return result >= 256 * 1024;
	}
	
#endif
	
}
