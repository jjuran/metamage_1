/*
	copy_bundle_icon.cc
	-------------------
*/

#include "mac_app/copy_bundle_icon.hh"

// mac-icon-utils
#include "mac_icon/create_IconRef_from_icns.hh"


#pragma exceptions off


namespace mac {
namespace app {

static
CFURLRef CopyBundleIconURL()
{
	CFURLRef url = NULL;
	
	CFBundleRef bundle = CFBundleGetMainBundle();
	
	CFStringRef key = CFSTR( "CFBundleIconFile" );
	
	CFTypeRef value = CFBundleGetValueForInfoDictionaryKey( bundle, key );
	
	if ( CFStringRef name = (CFStringRef) value )
	{
		url = CFBundleCopyResourceURL( bundle, name, NULL, NULL );
	}
	
	return url;
}

IconRef copy_bundle_icon()
{
	IconRef icon = NULL;
	
	if ( CFURLRef url = CopyBundleIconURL() )
	{
		FSRef file;
		
		if ( CFURLGetFSRef( url, &file ) )
		{
			icon = mac::icon::create_IconRef_from_icns( file );
		}
		
		CFRelease( url );
		
		return icon;
	}
	
#if TARGET_RT_MAC_MACHO
	
	/*
		Only substitute the generic application in Mach-O builds.
		For Carbon CFM, fall back to drawing an icon resource.
	*/
	
	const OSType type = kGenericApplicationIcon;
	
	OSErr err = GetIconRef( kOnSystemDisk, kSystemIconsCreator, type, &icon );
	
	if ( err == noErr )
	{
		AcquireIconRef( icon );
	}
	
#endif
	
	return icon;
}

}
}
