/*
	Mac/AppleEvents/Types/AEEventID.hh
	----------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTID_HH
#define MAC_APPLEEVENTS_TYPES_AEEVENTID_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEEventID
	{
		kAEEventID_Max = nucleus::enumeration_traits< ::AEEventID >::max
	};
	
}

#endif

