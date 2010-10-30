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
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum ResType
	{
		kVersionResType = 'vers',
		
		kResType_Max = nucleus::enumeration_traits< ::ResType >::max
	};
	
}

#endif

