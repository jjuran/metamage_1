/*
	ADB.cc
	------
*/

#include "MacFeatures/ADB.hh"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace MacFeatures
{
	
#if TARGET_CPU_68K  &&  !__MC68020__
	
	bool Has_ADB()
	{
		long result;
		
		const OSErr err = ::Gestalt( gestaltROMSize, &result );
		
		// 256K ROM has ADB
		return err == noErr  &&  result >= 256 * 1024;
	}
	
#endif
	
}

