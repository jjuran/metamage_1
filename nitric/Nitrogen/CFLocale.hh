// Nitrogen/CFLocale.hh
// --------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004 by Marshall Clow.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_CFLOCALE_HH
#define NITROGEN_CFLOCALE_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __COREFOUNDATION_CFLOCALE__
#include <CoreFoundation/CFLocale.h>
#endif

#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

namespace Nitrogen {
	using ::CFLocaleRef;
  }

namespace nucleus
  {
	template <> struct disposer_traits< Nitrogen::CFLocaleRef >: disposer_traits< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {
	template <> struct CFType_Traits< CFLocaleRef >: Basic_CFType_Traits< CFLocaleRef, ::CFLocaleGetTypeID > {};

//	CFLocaleRef CFLocaleGetSystem ( void );
	using ::CFLocaleGetSystem;
	

	inline nucleus::owned<CFLocaleRef> CFLocaleCopyCurrent ( void ) {
		return nucleus::owned<CFLocaleRef>::seize ( ::CFLocaleCopyCurrent ());
		}
		
	inline nucleus::owned<CFStringRef> CFLocaleCreateCanonicalLocaleIdentifierFromString (
			CFAllocatorRef allocator, CFStringRef localeIdentifier ) {
		return nucleus::owned<CFStringRef>::seize ( ::CFLocaleCreateCanonicalLocaleIdentifierFromString ( allocator, localeIdentifier ));
		}
	
	inline nucleus::owned<CFStringRef> CFLocaleCreateCanonicalLocaleIdentifierFromScriptManagerCodes (
			CFAllocatorRef allocator, LangCode lCode, RegionCode rCode ) {
		return nucleus::owned<CFStringRef>::seize ( ::CFLocaleCreateCanonicalLocaleIdentifierFromScriptManagerCodes ( allocator, lCode, rCode ));
		}

	
	inline nucleus::owned<CFLocaleRef> CFLocaleCreate ( CFAllocatorRef allocator, CFStringRef localeIdentifier) {
		return nucleus::owned<CFLocaleRef>::seize ( ::CFLocaleCreate ( allocator, localeIdentifier ));
		}
		
	inline nucleus::owned<CFLocaleRef> CFLocaleCreateCopy ( CFAllocatorRef allocator, CFLocaleRef locale ) {
		return nucleus::owned<CFLocaleRef>::seize ( ::CFLocaleCreateCopy ( allocator, locale ));
		}

//	CFStringRef CFLocaleGetIdentifier ( CFLocaleRef locale );
	using ::CFLocaleGetIdentifier;
	
//	CFTypeRef CFLocaleGetValue ( CFLocaleRef locale, CFStringRef key );
	using ::CFLocaleGetValue;
	}

#endif
