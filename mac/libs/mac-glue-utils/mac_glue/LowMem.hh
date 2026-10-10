/*
	LowMem.hh
	---------
*/

#ifndef MACGLUE_LOWMEM_HH
#define MACGLUE_LOWMEM_HH

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


namespace mac  {
namespace glue {
	
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
