// Nostalgia/MacWindows.hh

#ifndef NOSTALGIA_LOWMEM_HH
#define NOSTALGIA_LOWMEM_HH

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if TARGET_API_MAC_CARBON
	
	inline pascal UInt32 LMGetTicks()
	{
		return ::TickCount();
	}
	
#endif

#ifdef __cplusplus
}
#endif

#endif

