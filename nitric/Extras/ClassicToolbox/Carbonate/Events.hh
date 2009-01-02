// Carbonate/Events.hh

#ifndef CARBONATE_EVENTS_HH
#define CARBONATE_EVENTS_HH

#ifndef __EVENTS__
#include <Events.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONATE_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONATE_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONATE_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONATE_LINKAGE
#define CARBONATE_LINKAGE inline pascal
#endif

CARBONATE_LINKAGE void GetGlobalMouse( Point* globalMouse )
{
	const EventMask kNoEvents = 0;
	
	EventRecord event;
	
	(void) OSEventAvail( kNoEvents, &event );
	
	*globalMouse = event.where;
}

#undef CARBONATE_LINKAGE

#endif

#endif

