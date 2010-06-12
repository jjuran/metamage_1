/*
	Mac/Script/Types/RegionCode.hh
	------------------------------
*/

#ifndef MAC_SCRIPT_TYPES_REGIONCODE_HH
#define MAC_SCRIPT_TYPES_REGIONCODE_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum RegionCode
	{
		verUS = ::verUS,
		
		kRegionCode_Max = nucleus::enumeration_traits< ::RegionCode >::max
	};
	
}

#endif

