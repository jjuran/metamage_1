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


#define LOWMEM( addr, type )  (*(type*) (addr))

#define ResErr  LOWMEM( 0x0A60, short )


namespace mac  {
namespace glue {
	
	inline
	short res_error()
	{
	#if TARGET_API_MAC_CARBON
		
		return LMGetResErr();
		
	#endif
		
		return ResErr;
	}
	
}
}

#endif
