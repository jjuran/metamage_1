/*	===========================
 *	FSTree_sys_mac_user_name.cc
 *	===========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_user_name.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/Resources.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static std::string GetStringResource( ::ResID id )
	{
		::Handle h = N::GetResource( N::ResType( 'STR ' ), N::ResID( id ) );
		
		std::size_t length = **h;
		
		const char* begin = *h + 1;
		
		std::string result;
		
		result.resize( length );
		
		std::copy( begin, begin + length, result.begin() );
		
		return result;
	}
	
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
	
	static UInt32 SystemVersion()
	{
		static UInt32 sysv = N::Gestalt( N::GestaltSelector( gestaltSystemVersion ) );
		
		return sysv;
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
		if ( !TARGET_RT_MAC_MACHO && (!TARGET_API_MAC_CARBON  ||  SystemVersion() < 0x00001000) )
		{
			return GetStringResource( -16096 );
		}
		
		if ( CFStringRef userName = CSCopyUserName_CFM( false ) )
		{
			return CFStringGetStdString( userName );
		}
		
		return "";
	}
	
	std::string sys_mac_user_name_Query::operator()() const
	{
		return GetUserName() + "\n";
	}
	
}

