/*	======================
 *	FSTree_sys_mac_name.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_name.hh"

// MacFeatures
#include "MacFeatures/Features.hh"

// Genie
#include "Genie/Utilities/CFStringGetStdString.hh"
#include "Genie/Utilities/GetStringResource.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	using MacFeatures::Is_Running_OSXNative;
	
	
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

