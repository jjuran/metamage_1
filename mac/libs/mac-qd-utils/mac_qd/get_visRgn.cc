/*
	get_visRgn.cc
	-------------
*/

#include "mac_qd/get_visRgn.hh"

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

::RgnHandle get_visRgn( GrafPtr port )
{
	static ::RgnHandle rgn = NewRgn();
	
	return GetPortVisibleRegion( port, rgn );
}

#endif  // #if ! OPAQUE_TOOLBOX_STRUCTS
#else  // #if ! __LP64__

int dummy;

#endif  // #if ! __LP64__
	
}
}
