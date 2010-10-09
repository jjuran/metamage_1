/*	===========
 *	Features.cc
 *	===========
 */

#include "MacFeatures/Features.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"
#include "MacFeatures/SystemVersion.hh"


namespace MacFeatures
{
	
#if !TARGET_RT_MAC_MACHO  &&  TARGET_API_MAC_CARBON
	
	bool Is_Running_OSXNative()
	{
		return SystemVersion() >= 0x1000;
	}
	
#endif
	
#if !TARGET_RT_MAC_MACHO
	
	bool Has_OSXSystem()
	{
		const unsigned sysv = SystemVersion();
		
		if ( TARGET_API_MAC_CARBON  &&  sysv >= 0x1000 )
		{
			return true;
		}
		
		if ( sysv < 0x0900 )
		{
			return false;
		}
		
		return Is_BlueBoxed();
	}
	
#endif
	
}

