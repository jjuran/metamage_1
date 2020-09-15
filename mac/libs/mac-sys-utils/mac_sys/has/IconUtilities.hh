/*
	IconUtilities.hh
	----------------
*/

#ifndef MACSYS_HAS_ICONUTILITIES_HH
#define MACSYS_HAS_ICONUTILITIES_HH

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_IconUtilities()
	{
		enum
		{
			_IconDispatch = 0xABC9,
		};
		
		return TARGET_API_MAC_CARBON  ||  trap_available( _IconDispatch );
	}
	
}
}

#endif
