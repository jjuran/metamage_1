/*	===========================
 *	FSTree_sys_mac_user_name.cc
 *	===========================
 */

#include "Genie/FS/FSTree_sys_mac_user_name.hh"

// MacFeatures
#include "MacFeatures/Features.hh"

// Genie
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
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
	
	static std::string CFStringGetStdString( CFStringRef string )
	{
		CFIndex length = CFStringGetLength( string );
		
		std::string result;
		
		result.resize( length );
		
		CFIndex usedBufLen = 0;
		
		CFIndex nConverted = CFStringGetBytes( string,
		                                       CFRangeMake( 0, length ),
		                                       kCFStringEncodingMacRoman,
		                                       '\0',
		                                       false,
		                                       (UInt8*) &result[0],
		                                       length,
		                                       &usedBufLen );
		
		if ( usedBufLen != length )
		{
			throw N::ParamErr();
		}
		
		return result;
	}
	
	static std::string GetUserName()
	{
		if ( !Is_Running_OSXNative() )
		{
			return GetStringResource( -16096 );
		}
		
		if ( CFStringRef userName = CSCopyUserName_CFM( false ) )
		{
			return CFStringGetStdString( userName );
		}
		
		return "";
	}
	
	std::string sys_mac_user_name::Read( const FSTree* that, bool binary )
	{
		return GetUserName();
	}
	
}

