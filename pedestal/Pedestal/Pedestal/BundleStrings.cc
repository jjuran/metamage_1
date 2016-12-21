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

// Pedestal
#include "Pedestal/OwnerResource.hh"


namespace Pedestal
{
	
	static inline
	CFBundleRef MainBundle()
	{
		return CFBundleGetMainBundle();
	}
	
	static
	OSType GetCreator()
	{
		if ( TARGET_API_MAC_CARBON )
		{
			OSType creator;
			CFBundleGetPackageInfo( MainBundle(), NULL, &creator );
			
			return creator;
		}
		
		return GetCreatorFromBNDL();
	}
	
	static
	CFStringRef CopyOwnerResourceAsCFString()
	{
		Str255 data;
		
		if ( GetOwnerResourceData( GetCreator(), data ) )
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
	
	CFStringRef GetBundleGetInfoString()
	{
		CFStringRef key = CFSTR( "CFBundleGetInfoString" );
		CFStringRef alt = CFSTR( "" );
		
		CFStringRef string;
		
		return   (string = GetBundleString( key )      ) ? string
		       : (string = GetOwnerResourceAsCFString()) ? string
		       : alt;
	}
	
}
