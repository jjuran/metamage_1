/*
	temp_free_mem.cc
	----------------
*/

#include "mac_sys/temp_free_mem.hh"

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
	
	long temp_free_mem()
	{
		return ::TempFreeMem();
	}
	
#else
	
	void dummy()
	{
	}
	
#endif
	
}
}
