/*
	CursorDevices.hh
	----------------
*/

#ifndef MACFEATURES_CURSORDEVICES_HH
#define MACFEATURES_CURSORDEVICES_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


namespace MacFeatures
{
	
	bool Has_CursorDevices();
	
	
#if !TARGET_CPU_68K
	
	inline bool Has_CursorDevices()
	{
		/*
			All Power Macs have the Cursor Device Manager, but it's not
			available in Carbon
		*/
		return !TARGET_API_MAC_CARBON;
	}
	
#endif
	
}

#endif

