/*
	screenBits.hh
	-------------
*/

#ifndef MACQD_GLOBALS_SCREENBITS_HH
#define MACQD_GLOBALS_SCREENBITS_HH

// Mac OS
#ifndef __APPLE__
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#endif

struct BitMap;


namespace mac {
namespace qd  {


#if OPAQUE_TOOLBOX_STRUCTS  ||  TARGET_API_MAC_CARBON

const BitMap& screenBits();

#else

inline
const BitMap& screenBits()
{
	return ::qd.screenBits;
}
	
#endif
	
}
}

#endif
