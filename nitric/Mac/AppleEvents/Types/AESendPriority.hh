/*
	Mac/AppleEvents/Types/AESendPriority.hh
	---------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AESENDPRIORITY_HH
#define MAC_APPLEEVENTS_TYPES_AESENDPRIORITY_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace Mac
{
	
	enum AESendPriority
	{
		kAENormalPriority = ::kAENormalPriority,
		kAEHighPriority   = ::kAEHighPriority,
		
		kAESendPriority_Max = nucleus::enumeration_traits< ::AESendPriority >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AESendPriority )
	
}

#endif

