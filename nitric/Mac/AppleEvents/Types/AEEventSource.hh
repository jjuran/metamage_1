/*
	Mac/AppleEvents/Types/AEEventSource.hh
	--------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTSOURCE_HH
#define MAC_APPLEEVENTS_TYPES_AEEVENTSOURCE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEEventSource
	{
		kAEUnknownSource = ::kAEUnknownSource,
		kAEDirectCall    = ::kAEDirectCall,
		kAESameProcess   = ::kAESameProcess,
		kAELocalProcess  = ::kAELocalProcess,
		kAERemoteProcess = ::kAERemoteProcess,
		
		kAEEventSource_Max = nucleus::enumeration_traits< ::AEEventSource >::max
	};
	
}

#endif

