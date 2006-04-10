// CarbonUnits/Quickdraw.cc


#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONUNITS_LINKAGE pascal
#include "CarbonUnits/Quickdraw.hh"
#endif

// These functions are always declared in the headers and are always extern.

pascal Boolean QDSwapPort( CGrafPtr newCPort, CGrafPtr* oldCPortPtr )
{
	GrafPtr oldPort;
	GetPort( &oldPort );
	
	if ( oldCPortPtr != NULL )
	{
		*oldCPortPtr = reinterpret_cast< CGrafPtr >( oldPort );
	}
	
	GrafPtr newPort = reinterpret_cast< GrafPtr >( newCPort );
	
	SetPort( newPort );
	
	return oldPort != newPort;
}

