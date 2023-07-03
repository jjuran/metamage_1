/*
	system_file.cc
	--------------
*/

#include "mac_file/system_file.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if TARGET_API_MAC_CARBON
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#else
#define LMGetSysMap()  (*(short*) 0x0A58)
#endif

// mac-file-utils
#include "mac_file/refnum_file.hh"


namespace mac  {
namespace file {
	
	types::FSSpec system_file()
	{
		return refnum_file( LMGetSysMap() );
	}
	
}
}
