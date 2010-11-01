/*
	Genie/FS/sys/mac/user/name.hh
	-----------------------------
*/

#include "Genie/FS/sys/mac/user/name.hh"

// Mac OS
#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif
#ifndef __CFSTRING__
#include <CFString.h>
#endif

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/CFBase.hh"

// MacFeatures
#include "MacFeatures/Features.hh"

// Genie
#include "Genie/Utilities/CFStringGetStdString.hh"
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	using MacFeatures::Is_Running_OSXNative;
	
	
	static CFStringRef CSCopyUserName_CFM( Boolean useShortName )
	{
		typedef CFStringRef (*CSCopyUserNameProc)( Boolean useShortName );
		
		CFStringRef carbon = CFSTR( "com.apple.Carbon" );
		
		if ( CFBundleRef bundle = CFBundleGetBundleWithIdentifier( carbon ) )
		{
			CFStringRef name = CFSTR( "CSCopyUserName" );
			
			CSCopyUserNameProc csCopyUserName = (CSCopyUserNameProc) CFBundleGetFunctionPointerForName( bundle, name );
			
			if ( void* ptr = CFBundleGetFunctionPointerForName( bundle, name ) )
			{
				CSCopyUserNameProc csCopyUserName = (CSCopyUserNameProc) ptr;
				
				return csCopyUserName( useShortName );
			}
		}
		
		// Both bundle and csCopyUserName got with "Get", so
		// no need to release.
		
		return NULL;
	}
	
	static plus::string GetUserName( bool convert_to_UTF8 )
	{
		if ( !Is_Running_OSXNative() )
		{
			return GetStringResource( -16096 );
		}
		
		if ( CFStringRef name = CSCopyUserName_CFM( false ) )
		{
			CFStringEncoding encoding = convert_to_UTF8 ? kCFStringEncodingUTF8
			                                            : kCFStringEncodingMacRoman;
			
			return CFStringGetStdString( n::owned< CFStringRef >::seize( name ), encoding );
		}
		
		return "";
	}
	
	void sys_mac_user_macname::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		result = GetUserName( false );
	}
	
	void sys_mac_user_name::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		result = GetUserName( true );
	}
	
}

