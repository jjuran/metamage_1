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
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif


namespace mac {
namespace qd  {

#ifdef __MC68K__

inline
asm
GrafPtr* deref_A5()
{
	MOVEA.L  (A5),A0
}

#endif

#if ! __LP64__

inline
GrafPtr thePort()
{
#if OPAQUE_TOOLBOX_STRUCTS
	
	return (GrafPtr) GetQDGlobalsThePort();
	
#elif ! defined( __MC68K__ )  ||  __A5__
	
	return ::qd.thePort;
	
#else
	
	return (GrafPtr) *deref_A5();
	
#endif
}

#endif

}
}

#endif
