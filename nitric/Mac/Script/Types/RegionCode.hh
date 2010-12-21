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
#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum RegionCode
	{
		verUS = ::verUS,
		
		kTextRegionDontCare = ::kTextRegionDontCare,
		
		kRegionCode_Max = nucleus::enumeration_traits< ::RegionCode >::max
	};
	
}

#endif

