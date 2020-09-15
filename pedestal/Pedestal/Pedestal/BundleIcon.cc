/*
	BundleIcon.cc
	-------------
*/

#include "Pedestal/BundleIcon.hh"

// Nitrogen
#include "Nitrogen/CFURL.hh"
#include "Nitrogen/Icons.hh"

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
		
		return N::GetIconRef( Mac::FSType( kGenericApplicationIcon ) );
	}
	
}
