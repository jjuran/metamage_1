/*
	ADB.hh
	------
*/

#ifndef MACFEATURES_ADB_HH
#define MACFEATURES_ADB_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


namespace MacFeatures
{
	
	bool Has_ADB();
	
	
#if !TARGET_CPU_68K  ||  __MC68020__
	
	inline bool Has_ADB()
	{
		/*
			All OldWorld Macs with 68020 or better have ADB.
			Mac OS also provides ADB APIs for USB devices, but not in Carbon.
		*/
		
		return !TARGET_API_MAC_CARBON;
	}
	
#endif
	
}

#endif

