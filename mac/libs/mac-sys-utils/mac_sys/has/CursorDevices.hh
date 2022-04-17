/*
	CursorDevices.hh
	----------------
*/

#ifndef MACSYS_HAS_CURSORDEVICES_HH
#define MACSYS_HAS_CURSORDEVICES_HH

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_CursorDevices()
	{
		/*
			All Power Macs have the Cursor Device Manager, but it's not
			available in Carbon
		*/
		
	#if TARGET_API_MAC_CARBON
		
		return false;
		
	#endif
		
	#ifdef __POWERPC__
		
		return true;
		
	#endif
		
		enum
		{
			_CursorDeviceDispatch = 0xAADB,
		};
		
		return trap_available( _CursorDeviceDispatch );
	}
	
}
}

#endif
