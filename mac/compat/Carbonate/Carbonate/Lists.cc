// Carbonate/Lists.cc


#ifndef __LISTS__
#include <Lists.h>
#endif

#if !TARGET_API_MAC_CARBON

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/Lists.hh"
#endif

// These functions are always declared in the headers and are always extern.

#endif

