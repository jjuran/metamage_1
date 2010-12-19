/*
	Mac/AppleEvents/Types/AEEventClass.hh
	-------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTCLASS_HH
#define MAC_APPLEEVENTS_TYPES_AEEVENTCLASS_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEEventClass
	{
		kAEEventClass_Max = nucleus::enumeration_traits< ::AEEventClass >::max
	};
	
}

#endif

