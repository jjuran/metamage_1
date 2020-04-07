/*
	ADB.cc
	------
*/

#include "MacFeatures/ADB.hh"

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace MacFeatures
{
	
#if TARGET_CPU_68K  &&  !__MC68020__
	
	bool Has_ADB()
	{
		enum { _CountADBs = 0xA077 };
		
		return mac::sys::trap_available( _CountADBs );
	}
	
#endif
	
}
