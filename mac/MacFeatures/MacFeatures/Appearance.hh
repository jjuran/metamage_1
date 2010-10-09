/*
	Appearance.hh
	-------------
*/

#ifndef MACFEATURES_APPEARANCE_HH
#define MACFEATURES_APPEARANCE_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


namespace MacFeatures
{
	
	bool Has_Appearance();
	
	
#if TARGET_API_MAC_CARBON
	
	inline bool Has_Appearance()
	{
		// Carbon implies 8.1 or later; 8.0 or later implies Appearance.
		return true;
	}
	
#endif
	
}

#endif

