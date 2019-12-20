/*
	Genie/FS/sys/mac/user/name.hh
	-----------------------------
*/

#include "Genie/FS/sys/mac/user/name.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Mac OS
#ifndef __COREFOUNDATION_CFBUNDLE__
#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif
#endif
#ifndef __COREFOUNDATION_CFSTRING__
#ifndef __CFSTRING__
#include <CFString.h>
#endif
#endif

// plus
#include "plus/mac_utf8.hh"
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
			plus::string result = GetStringResource( -16096 );
			
			if ( convert_to_UTF8 )
			{
				result = plus::utf8_from_mac( result );
			}
			
			return result;
		}
		
		if ( CFStringRef name = CSCopyUserName_CFM( false ) )
		{
			CFStringEncoding encoding = convert_to_UTF8 ? kCFStringEncodingUTF8
			                                            : kCFStringEncodingMacRoman;
			
			return CFStringGetStdString( n::owned< CFStringRef >::seize( name ), encoding );
		}
		
		return plus::string::null;
	}
	
	static
	void sys_mac_user_macname_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		result = GetUserName( false );
	}
	
	static
	void sys_mac_user_name_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		result = GetUserName( true );
	}
	
	const vfs::property_params sys_mac_user_macname_params =
	{
		vfs::no_fixed_size,
		&sys_mac_user_macname_get,
	};
	
	const vfs::property_params sys_mac_user_name_params =
	{
		vfs::no_fixed_size,
		&sys_mac_user_name_get,
	};
	
}
