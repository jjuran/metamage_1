/*
	MenuItemCommands.hh
	-------------------
*/

#ifndef PEDESTAL_MENUITEMCOMMANDS_HH
#define PEDESTAL_MENUITEMCOMMANDS_HH

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
	
	CommandCode GetMenuItemCommandCode( Mac::MenuID    menu_id,
	                                    MenuItemIndex  item_index );
	
	void SetMenuItemCommandCode( Mac::MenuID    menu_id,
	                             MenuItemIndex  item_index,
	                             CommandCode    code );
	
}

#endif

