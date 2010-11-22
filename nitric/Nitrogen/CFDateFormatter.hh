// Nitrogen/CFDateFormatter.hh
// ---------------------------

// Part of the Nitrogen project.
//
// Written 2004-2007 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFDATEFORMATTER_HH
#define NITROGEN_CFDATEFORMATTER_HH

#ifndef __MACH__
#error "These routines are only directly callable from MachO"
#endif

#ifndef __COREFOUNDATION_CFDATEFORMATTER__
#include <CoreFoundation/CFDateFormatter.h>
#endif

#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif
#ifndef NITROGEN_CFDATE_HH
#include "Nitrogen/CFDate.hh"	// We need this for the defs for Owned < CFDateRef >
#endif

namespace Nitrogen {
	using ::CFDateFormatterRef;
   }

namespace nucleus
   {
	template <> struct disposer_class< CFDateFormatterRef >: disposer_class< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {
	template <> struct CFType_Traits< CFDateFormatterRef >: Basic_CFType_Traits< CFDateFormatterRef, ::CFDateFormatterGetTypeID > {};

	inline nucleus::owned<CFDateFormatterRef> CFDateFormatterCreate ( CFAllocatorRef allocator, 
			CFLocaleRef locale, CFDateFormatterStyle dateStyle, CFDateFormatterStyle timeStyle ) {
		return nucleus::owned<CFDateFormatterRef>::seize ( ::CFDateFormatterCreate ( allocator,
				locale, dateStyle, timeStyle ));
		}

	
//	CFLocaleRef CFDateFormatterGetLocale ( CFDateFormatterRef formatter );
	using ::CFDateFormatterGetLocale;

//	CFDateFormatterStyle CFDateFormatterGetDateStyle ( CFDateFormatterRef formatter );
	using ::CFDateFormatterGetDateStyle;

//	CFDateFormatterStyle CFDateFormatterGetTimeStyle ( CFDateFormatterRef formatter );
	using ::CFDateFormatterGetTimeStyle;
	
//	CFStringRef CFDateFormatterGetFormat ( CFDateFormatterRef formatter );
	using ::CFDateFormatterGetFormat;

//	void CFDateFormatterSetFormat ( CFDateFormatterRef formatter, CFStringRef formatString );
	using ::CFDateFormatterSetFormat;
	
	inline nucleus::owned<CFStringRef> CFDateFormatterCreateStringWithDate ( CFAllocatorRef allocator, CFDateFormatterRef formatter, CFDateRef date ) {
		return nucleus::owned<CFStringRef>::seize ( ::CFDateFormatterCreateStringWithDate ( allocator, formatter, date ));
		}
		

	inline nucleus::owned<CFStringRef> CFDateFormatterCreateStringWithAbsoluteTime ( CFAllocatorRef allocator, CFDateFormatterRef formatter, CFAbsoluteTime at ) {
		return nucleus::owned<CFStringRef>::seize ( ::CFDateFormatterCreateStringWithAbsoluteTime ( allocator, formatter, at ));
		}

	inline nucleus::owned<CFDateRef> CFDateFormatterCreateDateFromString ( CFAllocatorRef allocator, CFDateFormatterRef formatter, CFStringRef string, CFRange *rangep = NULL ) {
		return nucleus::owned<CFDateRef>::seize ( ::CFDateFormatterCreateDateFromString ( allocator, formatter, string, rangep ));
		}
		
	struct CFDateFormatterGetAbsoluteTimeFromString_Failed {};
	inline CFAbsoluteTime CFDateFormatterGetAbsoluteTimeFromString ( CFDateFormatterRef formatter, CFStringRef string, CFRange *rangep = NULL ) {
		CFAbsoluteTime result;
		if ( !::CFDateFormatterGetAbsoluteTimeFromString ( formatter, string, rangep, &result ))
			throw CFDateFormatterGetAbsoluteTimeFromString_Failed ();
		return result;
		}
	

//	void CFDateFormatterSetProperty ( CFDateFormatterRef formatter, CFStringRef key, CFTypeRef value );
	using ::CFDateFormatterSetProperty;
	
	inline nucleus::owned<CFTypeRef> CFDateFormatterCopyProperty ( CFDateFormatterRef formatter, CFStringRef key ) {
		return nucleus::owned<CFTypeRef>::seize ( ::CFDateFormatterCopyProperty ( formatter, key ));
		}
	
	}

#endif

