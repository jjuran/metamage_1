/*
	Mac/AppleEvents/Types/DescType_scribe_math.hh
	---------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBEMATH_HH
#define MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBEMATH_HH

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#include "Mac/AppleEvents/Types/DescType_scribe.hh"
#endif
#ifndef MAC_TOOLBOX_TYPES_FIXED_HH
#include "Mac/Toolbox/Types/Fixed.hh"
#endif


namespace Mac
{
	
	// AEDataModel
	// -----------
	
	template <> struct DescType_scribe< typeFixed > : type_< Fixed_scribe > {};
	
}

#endif

