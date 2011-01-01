/*
	Mac/AppleEvents/Types/AEKeyword_scribe_text.hh
	----------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBETEXT_HH
#define MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBETEXT_HH

// nucleus
#ifndef NUCLEUS_STRINGSCRIBE_HH
#include "nucleus/string_scribe.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBE_HH
#include "Mac/AppleEvents/Types/AEKeyword_scribe.hh"
#endif


namespace Mac
{
	
	// AppleEvents
	// -----------
	
	template <> struct AEKeyword_scribe< keyErrorString > : type_< nucleus::string_scribe > {};
	
}

#endif

