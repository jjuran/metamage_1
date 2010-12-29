/*
	Mac/AppleEvents/Types/AECompOperator.hh
	---------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AECOMPOPERATOR_HH
#define MAC_APPLEEVENTS_TYPES_AECOMPOPERATOR_HH

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AECompOperator
	{
		kAEBeginsWith        = ::kAEBeginsWith,
		kAECaseSensEquals    = ::kAECaseSensEquals,
		kAEContains          = ::kAEContains,
		kAEEndsWith          = ::kAEEndsWith,
		kAEEquals            = ::kAEEquals,
		kAEGreaterThan       = ::kAEGreaterThan,
		kAEGreaterThanEquals = ::kAEGreaterThanEquals,
		kAELessThan          = ::kAELessThan,
		kAELessThanEquals    = ::kAELessThanEquals,
		
		kAECompOperator_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
}

#endif

