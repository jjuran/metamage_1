/*	======================
 *	FSTree_sys_mac_name.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_name.hh"

// MacFeatures
#include "MacFeatures/Features.hh"

// Genie
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	using MacFeatures::Is_Running_OSXNative;
	
	
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
		if ( !Is_Running_OSXNative() )
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
	
	std::string sys_mac_name::Read( const FSTree* that, bool binary )
	{
		return GetMachineName();
	}
	
}

