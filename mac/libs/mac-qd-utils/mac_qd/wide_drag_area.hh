/*
	wide_drag_area.hh
	-----------------
*/

#ifndef MACQD_WIDEDRAGAREA_HH
#define MACQD_WIDEDRAGAREA_HH

// Mac OS
#ifndef __APPLE__
#if OPAQUE_TOOLBOX_STRUCTS  &&  ! TARGET_API_MAC_CARBON
	#ifndef __LOWMEM__
	#include <LowMem.h>
	#endif
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#endif


namespace mac {
namespace qd  {

inline
const Rect* wide_drag_area()
{
#if ! TARGET_API_MAC_CARBON
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	return &::qd.screenBits.bounds;
	
#else
	
	static Rect area;
	area = LMGetGrayRgn()[0]->rgnBBox;
	
	return &area;
	
#endif
#endif
	
	return 0;  // DragWindow() bounds may be NULL in Carbon
}

}
}

#endif
