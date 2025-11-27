/*
	lowmem.hh
	---------
*/

#ifndef MACSYS_HAS_LOWMEM_HH
#define MACSYS_HAS_LOWMEM_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if TARGET_API_MAC_CARBON
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#endif


namespace mac {
namespace sys {
	
	inline
	bool has_lowmem()
	{
	#ifdef __MACH__
		
		return false;
		
	#endif
		
	#if TARGET_API_MAC_CARBON
		
		SInt32 sysv;
		
		return Gestalt( 'sysv', &sysv ) == noErr  &&  sysv < 0x1000;
		
	#endif
		
		return true;
	}
	
}
}

#endif
