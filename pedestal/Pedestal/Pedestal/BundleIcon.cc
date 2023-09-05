/*
	BundleIcon.cc
	-------------
*/

#include "Pedestal/BundleIcon.hh"

// Nitrogen
#ifndef MAC_ICONS_TYPES_ICONREF_HH
#include "Mac/Icons/Types/IconRef.hh"
#endif

#include "Nitrogen/CFURL.hh"

// Pedestal
#include "Pedestal/IconRefFromIconFile.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static
	n::owned< CFURLRef > CopyBundleIconURL()
	{
		CFURLRef url = NULL;
		
		CFBundleRef bundle = CFBundleGetMainBundle();
		
		CFStringRef key = CFSTR( "CFBundleIconFile" );
		
		CFTypeRef value = CFBundleGetValueForInfoDictionaryKey( bundle, key );
		
		if ( CFStringRef name = (CFStringRef) value )
		{
			url = CFBundleCopyResourceURL( bundle, name, NULL, NULL );
		}
		
		return n::owned< CFURLRef >::seize( url );
	}
	
	n::owned< IconRef > BundleIcon()
	{
		if ( n::owned< CFURLRef > url = CopyBundleIconURL() )
		{
			return IconRefFromIconFile( N::CFURLGetFSRef( url ) );
		}
		
		enum
		{
			kOnSystemDisk = -32768L,
		};
		
		OSType creator  = kSystemIconsCreator;
		OSType iconType = kGenericApplicationIcon;
		
		IconRef icon;
		
		if ( OSErr err = GetIconRef( kOnSystemDisk, creator, iconType, &icon ) )
		{
			return n::owned< IconRef >();
		}
		
		return n::owned< IconRef >::seize( icon );
	}
	
}
