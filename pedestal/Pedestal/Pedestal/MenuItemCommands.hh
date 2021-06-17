/*
	MenuItemCommands.hh
	-------------------
*/

#ifndef PEDESTAL_MENUITEMCOMMANDS_HH
#define PEDESTAL_MENUITEMCOMMANDS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


namespace Pedestal
{
	
	CommandCode GetMenuItemCommandCode( MenuID         menu_id,
	                                    MenuItemIndex  item_index );
	
	void SetMenuItemCommandCode( MenuID         menu_id,
	                             MenuItemIndex  item_index,
	                             CommandCode    code );
	
}

#endif
