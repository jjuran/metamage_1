/*
	bundle_strings.hh
	-----------------
*/

#ifndef MACAPP_BUNDLESTRINGS_HH
#define MACAPP_BUNDLESTRINGS_HH

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


namespace mac {
namespace app {
	
	CFStringRef GetBundleName();
	CFStringRef GetBundleVersion();
	CFStringRef GetBundleGetInfoString();
	CFStringRef GetBundleShortVersionString();
	
}
}

#endif
