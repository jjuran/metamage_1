/*
	Appearance.cc
	-------------
*/

#include "MacFeatures/Appearance.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace MacFeatures
{
	
#if !TARGET_API_MAC_CARBON
	
	bool Has_Appearance()
	{
		enum { gestaltAppearanceAttr = 'appr' };
		
		return mac::sys::gestalt( gestaltAppearanceAttr );
	}
	
#endif
	
}
