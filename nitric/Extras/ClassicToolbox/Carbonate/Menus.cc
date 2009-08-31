// Carbonate/Menus.cc


#ifndef __MENUS__
#include <Menus.h>
#endif

#if !TARGET_API_MAC_CARBON

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/Menus.hh"
#endif

// These functions are always declared in the headers and are always extern.

#endif

