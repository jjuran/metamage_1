/*
	thePort.hh
	----------
*/

#ifndef MACQD_GLOBALS_THEPORT_HH
#define MACQD_GLOBALS_THEPORT_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


namespace mac {
namespace qd  {

#if ! __LP64__

inline
GrafPtr thePort()
{
#if ! (OPAQUE_TOOLBOX_STRUCTS  ||  TARGET_API_MAC_CARBON)
	
	return ::qd.thePort;
	
#endif
	
	return (GrafPtr) GetQDGlobalsThePort();
}

#endif

}
}

#endif
