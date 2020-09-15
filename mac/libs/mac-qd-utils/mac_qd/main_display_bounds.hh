/*
	main_display_bounds.hh
	----------------------
*/

#ifndef MACQD_MAINDISPLAYBOUNDS_HH
#define MACQD_MAINDISPLAYBOUNDS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __APPLE__
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#endif


namespace mac {
namespace qd  {

inline
const Rect& main_display_bounds()
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	return ::qd.screenBits.bounds;
	
#endif
	
	return GetMainDevice()[0]->gdRect;
}


}
}

#endif
