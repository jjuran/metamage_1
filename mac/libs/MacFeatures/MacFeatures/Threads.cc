/*
	Threads.cc
	----------
*/

#include "MacFeatures/Threads.hh"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace MacFeatures
{
	
#if !TARGET_API_MAC_CARBON
	
	bool Has_Threads()
	{
		long result;
		
		const OSErr err = ::Gestalt( gestaltThreadMgrAttr, &result );
		
		return err == noErr  &&  result & (1 << gestaltThreadMgrPresent);
	}
	
#endif
	
}

