/*
	heap_size.cc
	------------
*/

#include "mac_sys/heap_size.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


namespace mac {
namespace sys {
	
	long heap_size()
	{
	#if !TARGET_API_MAC_CARBON
		
		THz zone = ::ApplicationZone();
		
		return zone->bkLim - (Ptr) &zone->heapData;
		
	#endif
		
		return 0;
	}
	
}
}

