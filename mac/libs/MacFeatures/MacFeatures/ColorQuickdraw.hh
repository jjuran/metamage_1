/*
	ColorQuickdraw.hh
	-----------------
*/

#ifndef MACFEATURES_COLORQUICKDRAW_HH
#define MACFEATURES_COLORQUICKDRAW_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


namespace MacFeatures
{
	
	bool Has_ColorQuickdraw();
	
	
#if !TARGET_CPU_68K  ||  __MC68020__
	
	inline bool Has_ColorQuickdraw()
	{
		// All Macs with 68020 or better have Color QuickDraw
		return true;
	}
	
#endif
	
}

#endif

