/*
	Mac/AppleEvents/Types/DescType_scribe_text.hh
	---------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBETEXT_HH
#define MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBETEXT_HH

// nucleus
#ifndef NUCLEUS_STRINGSCRIBE_HH
#include "nucleus/string_scribe.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#include "Mac/AppleEvents/Types/DescType_scribe.hh"
#endif


namespace Mac
{
	
	// AEDataModel
	// -----------
	
	template <> struct DescType_scribe< typeText           > : nucleus::string_scribe {};
	template <> struct DescType_scribe< typeApplicationURL > : nucleus::string_scribe {};
	
}

#endif

