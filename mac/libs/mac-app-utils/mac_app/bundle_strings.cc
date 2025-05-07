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

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"


namespace mac {
namespace app {

static inline
CFBundleRef MainBundle()
{
	return CFBundleGetMainBundle();
}

static inline
OSType GetCreator()
{
	OSType creator;
	CFBundleGetPackageInfo( MainBundle(), NULL, &creator );
	
	return creator;
}

static
CFStringRef CopyOwnerResourceAsCFString()
{
	Str255 data;
	
	if ( mac::rsrc::get_string_from_resource( data, GetCreator(), 0 ) )
	{
		return CFStringCreateWithPascalString( NULL,
		                                       data,
		                                       kCFStringEncodingMacRoman );
	}
	
	return NULL;
}

static
CFStringRef Copy_vers_1_ResourceAsCFString()
{
	Str255 data;
	
	if ( mac::rsrc::get_vers_ShortVersionString( data, 1 ) )
	{
		return CFStringCreateWithPascalString( NULL,
		                                       data,
		                                       kCFStringEncodingMacRoman );
	}
	
	return NULL;
}

static
CFStringRef GetOwnerResourceAsCFString()
{
	static CFStringRef string = CopyOwnerResourceAsCFString();
	
	return string;
}

static
CFStringRef Get_vers_1_ResourceAsCFString()
{
	static CFStringRef string = Copy_vers_1_ResourceAsCFString();
	
	return string;
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
	
	CFStringRef string;
	
	return   (string = GetBundleString( key )      ) ? string
	       : (string = GetOwnerResourceAsCFString()) ? string
	       : alt;
}

CFStringRef GetBundleShortVersionString()
{
	CFStringRef key = CFSTR( "CFBundleShortVersionString" );
	
	CFStringRef string;
	
	return   (string = GetBundleString( key )         ) ? string
	       : (string = Get_vers_1_ResourceAsCFString()) ? string
	       : NULL;
}

}
}
