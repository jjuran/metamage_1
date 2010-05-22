/*
	Mac/Toolbox/Types/OSType.hh
	---------------------------
*/

#ifndef MAC_TOOLBOX_TYPES_OSTYPE_HH
#define MAC_TOOLBOX_TYPES_OSTYPE_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum OSType
	{
		kUnknownType = ::kUnknownType,
		
		kOSType_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
}

#endif

