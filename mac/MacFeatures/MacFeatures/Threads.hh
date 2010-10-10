/*
	Threads.hh
	----------
*/

#ifndef MACFEATURES_THREADS_HH
#define MACFEATURES_THREADS_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


namespace MacFeatures
{
	
	bool Has_Threads();
	
	
#if TARGET_API_MAC_CARBON
	
	inline bool Has_Threads()
	{
		// Carbon implies 8.1 or later; 7.5 or later implies Threads.
		return true;
	}
	
#endif
	
}

#endif

