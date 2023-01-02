/*
	screenBits.cc
	-------------
*/

#include "mac_qd/globals/screenBits.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
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

#if ! __LP64__
#if OPAQUE_TOOLBOX_STRUCTS

static BitMap qd_screenBits;

const BitMap& screenBits()
{
	GetQDGlobalsScreenBits( &qd_screenBits );
	
	return qd_screenBits;
}

#endif
#else  // #if ! __LP64__

int dummy;

#endif

}
}
