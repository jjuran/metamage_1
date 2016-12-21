/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// Nitrogen
#ifndef MAC_MENUS_TYPES_MENUID_HH
#include "Mac/Menus/Types/MenuID.hh"
#endif

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


namespace Pedestal
{
	
	inline
	bool HasAquaMenus()
	{
		if ( ! TARGET_API_MAC_CARBON )
		{
			return false;
		}
		
		const uint32_t gestaltMenuMgrAttr = 'menu';
		
		const int gestaltMenuMgrAquaLayoutBit = 1;
		
		return mac::sys::gestalt_bit_set( gestaltMenuMgrAttr,
		                                  gestaltMenuMgrAquaLayoutBit );
	}
	
	void AddMenu( MenuRef menu );
	
	void PopulateAppleMenu( MenuRef menu );
	
	CommandCode HandleMenuItem( Mac::MenuID menuID, SInt16 item );
	
}

#endif
