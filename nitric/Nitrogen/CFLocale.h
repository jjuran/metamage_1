// Nitrogen/CFLocale.h
// -------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004 by Marshall Clow.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_CFLOCALE_H
#define NITROGEN_CFLOCALE_H

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __COREFOUNDATION_CFLOCALE__
#include <CoreFoundation/CFLocale.h>
#endif

#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

namespace Nitrogen {
	using ::CFLocaleRef;
  }

namespace Nucleus
  {
	template <> struct Disposer_Traits< Nitrogen::CFLocaleRef >: Disposer_Traits< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {
	template <> struct CFType_Traits< CFLocaleRef >: Basic_CFType_Traits< CFLocaleRef, ::CFLocaleGetTypeID > {};

//	CFLocaleRef CFLocaleGetSystem ( void );
	using ::CFLocaleGetSystem;
	

	inline Nucleus::Owned<CFLocaleRef> CFLocaleCopyCurrent ( void ) {
		return Nucleus::Owned<CFLocaleRef>::Seize ( ::CFLocaleCopyCurrent ());
		}
		
	inline Nucleus::Owned<CFStringRef> CFLocaleCreateCanonicalLocaleIdentifierFromString (
			CFAllocatorRef allocator, CFStringRef localeIdentifier ) {
		return Nucleus::Owned<CFStringRef>::Seize ( ::CFLocaleCreateCanonicalLocaleIdentifierFromString ( allocator, localeIdentifier ));
		}
	
	inline Nucleus::Owned<CFStringRef> CFLocaleCreateCanonicalLocaleIdentifierFromScriptManagerCodes (
			CFAllocatorRef allocator, LangCode lCode, RegionCode rCode ) {
		return Nucleus::Owned<CFStringRef>::Seize ( ::CFLocaleCreateCanonicalLocaleIdentifierFromScriptManagerCodes ( allocator, lCode, rCode ));
		}

	
	inline Nucleus::Owned<CFLocaleRef> CFLocaleCreate ( CFAllocatorRef allocator, CFStringRef localeIdentifier) {
		return Nucleus::Owned<CFLocaleRef>::Seize ( ::CFLocaleCreate ( allocator, localeIdentifier ));
		}
		
	inline Nucleus::Owned<CFLocaleRef> CFLocaleCreateCopy ( CFAllocatorRef allocator, CFLocaleRef locale ) {
		return Nucleus::Owned<CFLocaleRef>::Seize ( ::CFLocaleCreateCopy ( allocator, locale ));
		}

//	CFStringRef CFLocaleGetIdentifier ( CFLocaleRef locale );
	using ::CFLocaleGetIdentifier;
	
//	CFTypeRef CFLocaleGetValue ( CFLocaleRef locale, CFStringRef key );
	using ::CFLocaleGetValue;
	}

#endif
