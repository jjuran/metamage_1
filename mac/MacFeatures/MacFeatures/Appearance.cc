/*
	Appearance.cc
	-------------
*/

#include "MacFeatures/Appearance.hh"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace MacFeatures
{
	
#if !TARGET_API_MAC_CARBON
	
	bool Has_Appearance()
	{
		long result;
		
		const OSErr err = ::Gestalt( gestaltAppearanceAttr, &result );
		
		return err == noErr  &&  result != 0;
	}
	
#endif
	
}

