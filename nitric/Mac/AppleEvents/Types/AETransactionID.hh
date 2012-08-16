/*
	Mac/AppleEvents/Types/AETransactionID.hh
	----------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AETRANSACTIONID_HH
#define MAC_APPLEEVENTS_TYPES_AETRANSACTIONID_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AETransactionID
	{
		kAnyTransactionID = ::kAnyTransactionID,
		
		kAETransactionID_Max = nucleus::enumeration_traits< ::AETransactionID >::max
	};
	
}

#endif

