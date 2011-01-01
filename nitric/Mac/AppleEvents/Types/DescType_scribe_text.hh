/*
	Mac/AppleEvents/Types/DescType_scribe_text.hh
	---------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBETEXT_HH
#define MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBETEXT_HH

// nucleus
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#include "Mac/AppleEvents/Types/DescType_scribe.hh"
#endif


namespace Mac
{
	
	typedef nucleus::POD_vector_scribe< nucleus::mutable_string > string_scribe;
	
	// AEDataModel
	// -----------
	
	template <> struct DescType_scribe< typeText           > : string_scribe {};
	template <> struct DescType_scribe< typeApplicationURL > : string_scribe {};
	
}

#endif

