// CFLocale.h

#ifndef NITROGEN_CFLOCALE_H
#define NITROGEN_CFLOCALE_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __COREFOUNDATION_CFLOCALE__
#include <CoreFoundation/CFLocale.h>
#endif

#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen {
	using ::CFLocaleRef;
	template <> struct OwnedDefaults< CFLocaleRef >: OwnedDefaults< CFTypeRef >  {};
	template <> struct CFType_Traits< CFLocaleRef >: Basic_CFType_Traits< CFLocaleRef, ::CFLocaleGetTypeID > {};

//	CFLocaleRef CFLocaleGetSystem ( void );
	using ::CFLocaleGetSystem;
	

	inline Owned<CFLocaleRef> CFLocaleCopyCurrent ( void ) {
		return Owned<CFLocaleRef>::Seize ( ::CFLocaleCopyCurrent ());
		}
		
	inline Owned<CFStringRef> CFLocaleCreateCanonicalLocaleIdentifierFromString (
			CFAllocatorRef allocator, CFStringRef localeIdentifier ) {
		return Owned<CFStringRef>::Seize ( ::CFLocaleCreateCanonicalLocaleIdentifierFromString ( allocator, localeIdentifier ));
		}
	
	inline Owned<CFStringRef> CFLocaleCreateCanonicalLocaleIdentifierFromScriptManagerCodes (
			CFAllocatorRef allocator, LangCode lCode, RegionCode rCode ) {
		return Owned<CFStringRef>::Seize ( ::CFLocaleCreateCanonicalLocaleIdentifierFromScriptManagerCodes ( allocator, lCode, rCode ));
		}

	
	inline Owned<CFLocaleRef> CFLocaleCreate ( CFAllocatorRef allocator, CFStringRef localeIdentifier) {
		return Owned<CFLocaleRef>::Seize ( ::CFLocaleCreate ( allocator, localeIdentifier ));
		}
		
	inline Owned<CFLocaleRef> CFLocaleCreateCopy ( CFAllocatorRef allocator, CFLocaleRef locale ) {
		return Owned<CFLocaleRef>::Seize ( ::CFLocaleCreateCopy ( allocator, locale ));
		}

//	CFStringRef CFLocaleGetIdentifier ( CFLocaleRef locale );
	using ::CFLocaleGetIdentifier;
	
//	CFTypeRef CFLocaleGetValue ( CFLocaleRef locale, CFStringRef key );
	using ::CFLocaleGetValue;
	}

#endif
