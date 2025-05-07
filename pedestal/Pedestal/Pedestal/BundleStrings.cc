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
	
	CFStringRef GetBundleName()
	{
		CFStringRef key = CFSTR( "CFBundleName" );
		
		return GetBundleString( key );
	}
	
}
