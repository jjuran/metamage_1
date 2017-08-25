/*
	ProcessManager.hh
	-----------------
*/

#ifndef MACSYS_HAS_PROCESSMANAGER_HH
#define MACSYS_HAS_PROCESSMANAGER_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_ProcessManager()
	{
		enum
		{
			gestaltOSAttr          = 'os  ',
			
			gestaltLaunchCanReturn = 1,
		};
		
		return gestalt_bit_set( gestaltOSAttr, gestaltLaunchCanReturn );
	}
	
}
}

#endif
