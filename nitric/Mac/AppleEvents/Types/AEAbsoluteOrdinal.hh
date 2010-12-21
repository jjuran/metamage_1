/*
	Mac/AppleEvents/Types/AEAbsoluteOrdinal.hh
	------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEABSOLUTEORDINAL_HH
#define MAC_APPLEEVENTS_TYPES_AEABSOLUTEORDINAL_HH

// Mac OS
#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEAbsoluteOrdinal
	{
		kAEFirst  = ::kAEFirst,
		kAELast   = ::kAELast,
		kAEMiddle = ::kAEMiddle,
		kAEAny    = ::kAEAny,
		kAEAll    = ::kAEAll,
		
		kAEAbsoluteOrdinal_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
}

#endif

