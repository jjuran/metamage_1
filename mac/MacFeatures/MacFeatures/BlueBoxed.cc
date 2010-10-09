/*
	BlueBoxed.cc
	------------
*/

#include "MacFeatures/BlueBoxed.hh"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace MacFeatures
{
	
#ifndef __MACH__
	
	bool Is_BlueBoxed()
	{
		long result;
		
		const OSErr err = ::Gestalt( gestaltMacOSCompatibilityBoxAttr, &result );
		
		return err == noErr  &&  result & (1 << gestaltMacOSCompatibilityBoxPresent);
	}
	
#endif
	
}

