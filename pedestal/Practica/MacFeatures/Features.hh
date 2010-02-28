/*	===========
 *	Features.hh
 *	===========
 */

#ifndef MACFEATURES_FEATURES_HH
#define MACFEATURES_FEATURES_HH

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
	
	inline UInt32 SystemVersion()
	{
		return Nitrogen::Gestalt( Nitrogen::gestaltSystemVersion );
	}
	
	inline bool Has_AppearanceManager()
	{
		// Carbon implies 8.1 or later; 8.0 or later implies Appearance.
		return TARGET_API_MAC_CARBON  ||  Nitrogen::Gestalt( Nitrogen::gestaltAppearanceAttr, 0 );
	}
	
	inline bool Is_Running_InClassic()
	{
		return !TARGET_RT_MAC_MACHO  &&  Nitrogen::Gestalt( Nitrogen::gestaltMacOSCompatibilityBoxAttr, 0 ) & gestaltMacOSCompatibilityBoxPresent;
	}
	
	inline bool Is_Running_OSXNative()
	{
		return TARGET_RT_MAC_MACHO  ||  TARGET_API_MAC_CARBON  &&  SystemVersion() >= 0x1000;
	}
	
	inline bool Has_OSXSystem()
	{
		if ( TARGET_RT_MAC_MACHO )
		{
			return true;
		}
		
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
	
}

#endif

