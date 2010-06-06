/*	===========
 *	Features.cc
 *	===========
 */

#include "MacFeatures/Features.hh"

// Nitrogen
#include "Nitrogen/Gestalt.hh"


namespace Nitrogen
{
	
	static const Gestalt_Selector gestaltAppearanceAttr            = Gestalt_Selector( ::gestaltAppearanceAttr            );
	static const Gestalt_Selector gestaltMacOSCompatibilityBoxAttr = Gestalt_Selector( ::gestaltMacOSCompatibilityBoxAttr );
	static const Gestalt_Selector gestaltSystemVersion             = Gestalt_Selector( ::gestaltSystemVersion             );
	
}

namespace MacFeatures
{
	
	namespace N = Nitrogen;
	
	
	UInt32 SystemVersion()
	{
		return N::Gestalt( N::gestaltSystemVersion );
	}
	
#if !TARGET_API_MAC_CARBON
	
	bool Has_AppearanceManager()
	{
		return N::Gestalt( N::gestaltAppearanceAttr, 0 );
	}
	
#endif
	
#if !TARGET_RT_MAC_MACHO
	
	bool Is_Running_InClassic()
	{
		return N::Gestalt( N::gestaltMacOSCompatibilityBoxAttr, 0 ) & gestaltMacOSCompatibilityBoxPresent;
	}
	
#endif
	
#if !TARGET_RT_MAC_MACHO  &&  TARGET_API_MAC_CARBON
	
	bool Is_Running_OSXNative()
	{
		return SystemVersion() >= 0x1000;
	}
	
#endif
	
#if !TARGET_RT_MAC_MACHO
	
	bool Has_OSXSystem()
	{
		const UInt32 sysv = SystemVersion();
		
		if ( TARGET_API_MAC_CARBON  &&  sysv >= 0x1000 )
		{
			return true;
		}
		
		if ( sysv < 0x0900 )
		{
			return false;
		}
		
		return Is_Running_InClassic();
	}
	
#endif
	
}

