/*
	arrow.hh
	--------
*/

#ifndef MACQD_GLOBALS_ARROW_HH
#define MACQD_GLOBALS_ARROW_HH

// Mac OS
#ifndef __APPLE__
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#endif

struct Cursor;


namespace mac {
namespace qd  {


#if OPAQUE_TOOLBOX_STRUCTS  ||  TARGET_API_MAC_CARBON

const Cursor& arrow();

#else

inline
const Cursor& arrow()
{
	return ::qd.arrow;
}
	
#endif
	
}
}

#endif
