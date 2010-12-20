/*
	Mac/AppleEvents/Types/AEEnumeration.hh
	--------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEENUMERATION_HH
#define MAC_APPLEEVENTS_TYPES_AEENUMERATION_HH

// Mac OS
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif


namespace Mac
{
	
	enum AEEnumeration
	{
		enumBooleanValues = ::enumBooleanValues,
		enumMiscValues    = ::enumMiscValues,
		
		kAEEnumeration_Max = 0xFFFFFFFFu
	};
	
}

#endif

