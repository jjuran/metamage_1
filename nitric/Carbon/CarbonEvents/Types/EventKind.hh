/*
	Carbon/CarbonEvents/Types/EventKind.hh
	--------------------------------------
*/

#ifndef MAC_CARBONEVENTS_TYPES_EVENTKIND_HH
#define MAC_CARBONEVENTS_TYPES_EVENTKIND_HH

#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Carbon
{
	
	enum EventKind
	{
		kEventProcessCommand      = ::kEventProcessCommand,
		kEventCommandUpdateStatus = ::kEventCommandUpdateStatus,
		
		kEventKind_Max = nucleus::enumeration_traits< UInt32 >::max
	};
	
}

#endif

