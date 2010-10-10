/*
	ColorQuickdraw.cc
	-----------------
*/

#include "MacFeatures/ColorQuickdraw.hh"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace MacFeatures
{
	
#if TARGET_CPU_68K  &&  !__MC68020__
	
	bool Has_ColorQuickdraw()
	{
		long result;
		
		const OSErr err = ::Gestalt( gestaltQuickdrawVersion, &result );
		
		return err == noErr  &&  result != gestaltOriginalQD;
	}
	
#endif
	
}

