/*
	native_Carbon.hh
	----------------
*/

#ifndef MACSYS_HAS_NATIVECARBON_HH
#define MACSYS_HAS_NATIVECARBON_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_native_Carbon()
	{
	#if TARGET_API_MAC_CARBON
		
		enum
		{
			gestaltSystemVersion = 'sysv',
		};
		
		return gestalt( gestaltSystemVersion ) >= 0x1000;
		
	#endif
		
		return false;
	}
	
}
}

#endif
