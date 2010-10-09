/*	===========
 *	Features.hh
 *	===========
 */

#ifndef MACFEATURES_FEATURES_HH
#define MACFEATURES_FEATURES_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


namespace MacFeatures
{
	
	bool Is_Running_OSXNative();
	
	bool Has_OSXSystem();
	
	
#if TARGET_RT_MAC_MACHO  ||  !TARGET_API_MAC_CARBON
	
	inline bool Is_Running_OSXNative()
	{
		// Mach-O is always OS-X-native, non-Carbon is never OS-X-native
		return TARGET_RT_MAC_MACHO;
	}
	
#endif
	
#if TARGET_RT_MAC_MACHO
	
	inline bool Has_OSXSystem()
	{
		// Anything that runs in OS 9 is indeterminate since it can run in
		// Classic, but Mach-O is always OS X.
		
		return true;
	}
	
#endif
	
}

#endif

