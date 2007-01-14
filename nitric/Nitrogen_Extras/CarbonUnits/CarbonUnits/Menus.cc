// CarbonUnits/Menus.cc


#ifndef __MENUS__
#include <Menus.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONUNITS_LINKAGE pascal
#include "CarbonUnits/Menus.hh"
#endif

// These functions are always declared in the headers and are always extern.



