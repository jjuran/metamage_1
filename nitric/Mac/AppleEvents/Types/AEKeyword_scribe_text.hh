/*
	Mac/AppleEvents/Types/AEKeyword_scribe_text.hh
	----------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBETEXT_HH
#define MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBETEXT_HH

// nucleus
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBE_HH
#include "Mac/AppleEvents/Types/AEKeyword_scribe.hh"
#endif


namespace Mac
{
	
	typedef nucleus::string_scribe< nucleus::mutable_string > string_scribe;
	
	// AppleEvents
	// -----------
	
	template <> struct AEKeyword_scribe< keyErrorString > : type_< string_scribe > {};
	
}

#endif

