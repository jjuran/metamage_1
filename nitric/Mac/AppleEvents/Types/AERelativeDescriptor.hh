/*
	Mac/AppleEvents/Types/AERelativeDescriptor.hh
	---------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AERELATIVEDESCRIPTOR_HH
#define MAC_APPLEEVENTS_TYPES_AERELATIVEDESCRIPTOR_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

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
	
	enum AERelativeDescriptor
	{
		kAENext     = ::kAENext,
		kAEPrevious = ::kAEPrevious,
		
		kAERelativeDescriptor_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
}

#endif

