/*
	Mac/Resources/Types/ResType.hh
	------------------------------
*/

#ifndef MAC_RESOURCES_TYPES_RESTYPE_HH
#define MAC_RESOURCES_TYPES_RESTYPE_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum ResType
	{
		kVersionResType = 'vers',
		
		kResType_Max = nucleus::enumeration_traits< ::ResType >::max
	};
	
}

#endif

