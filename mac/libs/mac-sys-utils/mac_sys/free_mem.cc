/*
	free_mem.cc
	-----------
*/

#include "mac_sys/free_mem.hh"

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
	
#if ! __LP64__
	
	long free_mem()
	{
		return ::FreeMem();
	}
	
#else
	
	void dummy()
	{
	}
	
#endif
	
}
}
