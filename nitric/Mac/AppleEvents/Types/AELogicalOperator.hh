/*
	Mac/AppleEvents/Types/AELogicalOperator.hh
	------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AELOGICALOPERATOR_HH
#define MAC_APPLEEVENTS_TYPES_AELOGICALOPERATOR_HH

#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AELogicalOperator
	{
		kAEAND = ::kAEAND,
		kAEOR  = ::kAEOR,
		kAENOT = ::kAENOT,
		
		kAELogicalOperator_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
}

#endif

