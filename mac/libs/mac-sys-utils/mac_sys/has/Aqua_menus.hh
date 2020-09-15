/*
	Aqua_menus.hh
	-------------
*/

#ifndef MACSYS_HAS_AQUAMENUS_HH
#define MACSYS_HAS_AQUAMENUS_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_Aqua_menus()
	{
		if ( ! TARGET_API_MAC_CARBON )
		{
			return false;
		}
		
		enum
		{
			gestaltMenuMgrAttr          = 'menu',
			
			gestaltMenuMgrAquaLayoutBit = 1,
		};
		
		return gestalt_bit_set( gestaltMenuMgrAttr,
		                        gestaltMenuMgrAquaLayoutBit );
	}
	
}
}

#endif
