/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// Nitrogen
#ifndef MAC_MENUS_TYPES_MENUID_HH
#include "Mac/Menus/Types/MenuID.hh"
#endif

// Pedestal
#include "Pedestal/CommandCode.hh"


namespace Pedestal
{
	
	void AddMenu( Mac::MenuID menuID );
	
	void PopulateAppleMenu( Mac::MenuID menuID );
	
	CommandCode HandleMenuItem( Mac::MenuID menuID, SInt16 item );
	
}

#endif

