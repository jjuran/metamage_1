/*
	res_error.hh
	------------
*/

#ifndef MACSYS_RESERROR_HH
#define MACSYS_RESERROR_HH

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
	short res_error()
	{
	#if ! TARGET_API_MAC_CARBON
		
		return *(short*) 0xA60;  // ResErr
		
	#else
		
		return LMGetResErr();
		
	#endif
	}
	
}
}

#endif
