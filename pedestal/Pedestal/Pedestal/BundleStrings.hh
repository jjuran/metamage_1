/*
	BundleStrings.hh
	----------------
*/

#ifndef PEDESTAL_BUNDLESTRINGS_HH
#define PEDESTAL_BUNDLESTRINGS_HH

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


#define VERSION_FALLBACK  "(A work in progress)"


namespace Pedestal
{
	
	CFStringRef GetBundleName();
	CFStringRef GetBundleVersion();
	CFStringRef GetBundleGetInfoString();
	CFStringRef GetBundleShortVersionString();
	
}

#endif
