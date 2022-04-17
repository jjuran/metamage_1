/*
	Appearance.hh
	-------------
*/

#ifndef MACSYS_HAS_APPEARANCE_HH
#define MACSYS_HAS_APPEARANCE_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_Appearance()
	{
	#if TARGET_API_MAC_CARBON
		
		// Carbon implies 8.1 or later; 8.0 or later implies Appearance.
		return true;
		
	#endif
		
		enum
		{
			gestaltAppearanceAttr = 'appr',
		};
		
		return gestalt( gestaltAppearanceAttr );
	}
	
}
}

#endif
