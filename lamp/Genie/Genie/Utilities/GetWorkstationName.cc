/*	=====================
 *	GetWorkstationName.cc
 *	=====================
 */

#include "Genie/Utilities/GetWorkstationName.hh"

// plus
#include "plus/mac_utf8.hh"

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
	
	
	static const void* kUnresolvedCFragSymbolAddress = NULL;
	
	
	plus::string GetWorkstationName( bool convert_to_UTF8 )
	{
		if ( !Is_Running_OSXNative() )
		{
			plus::string result = GetStringResource( -16413 );
			
			if ( convert_to_UTF8 )
			{
				result = plus::utf8_from_mac( result );
			}
			
			return result;
		}
		
		if ( CSCopyMachineName != kUnresolvedCFragSymbolAddress )
		{
			if ( CFStringRef name = CSCopyMachineName() )
			{
				CFStringEncoding encoding = convert_to_UTF8 ? kCFStringEncodingUTF8
				                                            : kCFStringEncodingMacRoman;
				
				return CFStringGetStdString( n::owned< CFStringRef >::seize( name ), encoding );
			}
		}
		
		return "";
	}
	
}

