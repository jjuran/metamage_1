// Carbonate/Files.cc


#ifndef __FILES__
#include <Files.h>
#endif

#if !TARGET_API_MAC_CARBON

struct FSRefPeek
{
	SInt16 vRefNum;    //  0
	SInt16 reserved1;  //  2
	SInt32 reserved2;  //  4
	UInt32 dirID;      //  8
	SInt32 reserved3;  // 12
	Str63  name;       // 16
};

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#include "Carbonate/Files.hh"
#endif

// These functions are always declared in the headers and are always extern.

#endif

