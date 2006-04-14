// CarbonUnits/Events.hh

#ifndef CARBONUNITS_EVENTS_HH
#define CARBONUNITS_EVENTS_HH

#ifndef __EVENTS__
#include <Events.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONUNITS_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONUNITS_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONUNITS_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONUNITS_LINKAGE
#define CARBONUNITS_LINKAGE inline
#endif

CARBONUNITS_LINKAGE void GetGlobalMouse( Point* globalMouse )
{
	GetMouse     ( globalMouse );
	LocalToGlobal( globalMouse );
}

#undef CARBONUNITS_LINKAGE

#endif

#endif

