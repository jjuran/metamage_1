// Carbonate/Lists.cc


#include <Lists.h>

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/Lists.hh"
#endif

// These functions are always declared in the headers and are always extern.

