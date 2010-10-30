/*
	Carbon/CF/Types/CFMutableStringRef.hh
	-------------------------------------
*/

#ifndef CARBON_CF_TYPES_CFMUTABLESTRINGREF_HH
#define CARBON_CF_TYPES_CFMUTABLESTRINGREF_HH

#ifndef __CFSTRING__
#include <CFString.h>
#endif

// nucleus
#include "nucleus/owned.hh"

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
	
	template <> struct disposer_traits< CFMutableStringRef > : disposer_traits< Carbon::CFTypeRef > {};
	
}

#endif

