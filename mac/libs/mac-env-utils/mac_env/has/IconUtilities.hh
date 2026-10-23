/*
	IconUtilities.hh
	----------------
*/

#ifndef MACENV_HAS_ICONUTILITIES_HH
#define MACENV_HAS_ICONUTILITIES_HH

// mac-env-utils
#include "mac_env/trap_available.hh"


namespace mac {
namespace env {
	
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
