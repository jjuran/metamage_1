/*
	Mac/Resources/Types/ResID.hh
	----------------------------
*/

#ifndef MAC_RESOURCES_TYPES_RESID_HH
#define MAC_RESOURCES_TYPES_RESID_HH

#ifndef __RESOURCES__
#include <Resources.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum ResID
	{
		kResID_Max = nucleus::enumeration_traits< ::ResID >::max
	};
	
}

#endif

