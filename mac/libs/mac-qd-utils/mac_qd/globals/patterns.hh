/*
	patterns.hh
	-----------
*/

#ifndef MACQD_GLOBALS_PATTERNS_HH
#define MACQD_GLOBALS_PATTERNS_HH

// Mac OS
#ifndef __APPLE__
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#endif

struct Pattern;


namespace mac {
namespace qd  {


#if OPAQUE_TOOLBOX_STRUCTS  ||  TARGET_API_MAC_CARBON

const Pattern& dkGray();
const Pattern& ltGray();
const Pattern& gray();
const Pattern& black();
const Pattern& white();

#else

inline const Pattern& dkGray()  { return ::qd.dkGray; }
inline const Pattern& ltGray()  { return ::qd.ltGray; }
inline const Pattern& gray()    { return ::qd.gray;   }
inline const Pattern& black()   { return ::qd.black;  }
inline const Pattern& white()   { return ::qd.white;  }
	
#endif
	
}
}

#endif
