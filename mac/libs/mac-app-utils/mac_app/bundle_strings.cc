/*
	bundle_strings.cc
	-----------------
*/

#include "mac_app/bundle_strings.hh"

// Mac OS
#ifndef __COREFOUNDATION_CFBUNDLE__
#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif
#endif


namespace mac {
namespace app {

static inline
CFBundleRef MainBundle()
{
	return CFBundleGetMainBundle();
}

static
CFStringRef GetBundleString( CFStringRef key )
{
	CFTypeRef value = CFBundleGetValueForInfoDictionaryKey( MainBundle(), key );
	
	return (CFStringRef) value;
}

CFStringRef GetBundleName()
{
	CFStringRef key = CFSTR( "CFBundleName" );
	
	return GetBundleString( key );
}

CFStringRef GetBundleVersion()
{
	CFStringRef key = CFSTR( "CFBundleVersion" );
	
	return GetBundleString( key );
}

CFStringRef GetBundleGetInfoString()
{
	CFStringRef key = CFSTR( "CFBundleGetInfoString" );
	CFStringRef alt = CFSTR( "" );
	
	CFStringRef string = GetBundleString( key );
	
	return string ? string : alt;
}

CFStringRef GetBundleShortVersionString()
{
	CFStringRef key = CFSTR( "CFBundleShortVersionString" );
	
	return GetBundleString( key );
}

}
}
