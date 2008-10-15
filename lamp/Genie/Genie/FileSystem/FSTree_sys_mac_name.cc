/*	======================
 *	FSTree_sys_mac_name.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_name.hh"

// Nitrogen
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
	
	static std::string GetMachineName()
	{
		if ( !TARGET_API_MAC_CARBON  ||  !TARGET_RT_MAC_MACHO  &&  SystemVersion() < 0x00001000 )
		{
			return GetStringResource( -16413 );
		}
		
		if ( CSCopyMachineName != (void*) kUnresolvedCFragSymbolAddress )
		{
			if ( CFStringRef userName = CSCopyMachineName() )
			{
				return CFStringGetStdString( userName );
			}
		}
		
		return "";
	}
	
	std::string sys_mac_name_Query::operator()() const
	{
		return GetMachineName() + "\n";
	}
	
}

