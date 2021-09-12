/*
	arrow.cc
	--------
*/

#include "mac_qd/globals/arrow.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
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
#if OPAQUE_TOOLBOX_STRUCTS

static Cursor qd_arrow;

const Cursor& arrow()
{
	GetQDGlobalsArrow( &qd_arrow );
	
	return qd_arrow;
}

#endif
#endif

int dummy;

}
}
