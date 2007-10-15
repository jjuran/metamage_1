// Carbonate/MacWindows.cc


#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
//#include "Carbonate/MacWindows.hh"
#endif


#if TARGET_CPU_68K

pascal WindowRef GetWindowList()
{
	// Not quite right, but close enough for now
	return FrontWindow();
}

#endif

