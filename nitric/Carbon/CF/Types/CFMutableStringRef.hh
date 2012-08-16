/*
	Carbon/CF/Types/CFMutableStringRef.hh
	-------------------------------------
*/

#ifndef CARBON_CF_TYPES_CFMUTABLESTRINGREF_HH
#define CARBON_CF_TYPES_CFMUTABLESTRINGREF_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Mac OS
#ifndef __COREFOUNDATION_CFSTRING__
#ifndef __CFSTRING__
#include <CFString.h>
#endif
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef CARBON_CF_TYPES_CFTYPEREF_HH
#include "Carbon/CF/Types/CFTypeRef.hh"
#endif


namespace Nitrogen
{
	
	template <> struct CFType_Traits< CFMutableStringRef > : Basic_CFType_Traits< CFMutableStringRef, ::CFStringGetTypeID > {};
	
}

namespace nucleus
{
	
	template <> struct disposer_class< CFMutableStringRef > : disposer_class< Carbon::CFTypeRef > {};
	
}

#endif

