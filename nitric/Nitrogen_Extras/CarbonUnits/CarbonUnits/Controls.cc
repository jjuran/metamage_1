// CarbonUnits/Controls.cc


#ifndef __CONTROLS__
#include <Controls.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONUNITS_LINKAGE pascal
#include "CarbonUnits/Controls.hh"
#endif

// These functions are always declared in the headers and are always extern.


