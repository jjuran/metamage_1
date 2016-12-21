/*
	BundleStrings.cc
	----------------
*/

#include "Pedestal/BundleStrings.hh"

// Mac OS
#ifndef __COREFOUNDATION_CFBUNDLE__
#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif
#endif


namespace Pedestal
{
	
	static inline
	CFBundleRef MainBundle()
	{
		return CFBundleGetMainBundle();
	}
	
	static
	CFStringRef GetBundleString( CFStringRef key )
	{
		CFTypeRef value;
		value = CFBundleGetValueForInfoDictionaryKey( MainBundle(), key );
		
		return (CFStringRef) value;
	}
	
	static
	CFStringRef GetBundleString( CFStringRef key, CFStringRef alternate )
	{
		CFStringRef value = GetBundleString( key );
		
		return value ? value : alternate;
	}
	
	CFStringRef GetBundleName()
	{
		CFStringRef key = CFSTR( "CFBundleName" );
		
		return GetBundleString( key );
	}
	
	CFStringRef GetBundleVersion()
	{
		CFStringRef key = CFSTR( "CFBundleVersion" );
		CFStringRef alt = CFSTR( "(A work in progress)" );
		
		return GetBundleString( key, alt );
	}
	
}
