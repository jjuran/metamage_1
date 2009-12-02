/*	=====================
 *	GetWorkstationName.cc
 *	=====================
 */

#include "Genie/Utilities/GetWorkstationName.hh"

// MacFeatures
#include "MacFeatures/Features.hh"

// Genie
#include "Genie/Utilities/CFStringGetStdString.hh"
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	using MacFeatures::Is_Running_OSXNative;
	
	
	static const void* kUnresolvedCFragSymbolAddress = NULL;
	
	
	std::string GetWorkstationName()
	{
		if ( !Is_Running_OSXNative() )
		{
			return GetStringResource( -16413 );
		}
		
		if ( CSCopyMachineName != kUnresolvedCFragSymbolAddress )
		{
			if ( CFStringRef userName = CSCopyMachineName() )
			{
				return CFStringGetStdString( userName );
			}
		}
		
		return "";
	}
	
}

