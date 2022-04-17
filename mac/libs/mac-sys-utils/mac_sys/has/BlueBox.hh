/*
	BlueBox.hh
	----------
*/

#ifndef MACSYS_HAS_BLUEBOX_HH
#define MACSYS_HAS_BLUEBOX_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_BlueBox()
	{
	#ifndef __MACH__
		
		enum
		{
			gestaltMacOSCompatibilityBoxAttr = 'bbox',
		};
		
		return gestalt( gestaltMacOSCompatibilityBoxAttr );
		
	#endif
		
		return false;
	}
	
}
}

#endif
