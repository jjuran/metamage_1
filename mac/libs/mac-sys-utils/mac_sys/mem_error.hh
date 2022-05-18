/*
	mem_error.hh
	------------
*/

#ifndef MACSYS_MEMERROR_HH
#define MACSYS_MEMERROR_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if TARGET_API_MAC_CARBON
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#endif


namespace mac {
namespace sys {
	
	inline
	short mem_error()
	{
	#if ! TARGET_API_MAC_CARBON
		
		return *(short*) 0x220;  // MemErr
		
	#else
		
		return LMGetMemErr();
		
	#endif
	}
	
}
}

#endif
