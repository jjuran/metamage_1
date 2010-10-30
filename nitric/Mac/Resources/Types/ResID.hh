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
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum ResID
	{
		kResID_Max = nucleus::enumeration_traits< ::ResID >::max
	};
	
}

#endif

