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
	
	CommandCode GetMenuItemCommandCode( long menu_choice );
	
	inline
	CommandCode GetMenuItemCommandCode( MenuID         menu_id,
	                                    MenuItemIndex  item_index )
	{
		long menu_choice = menu_id << 16 | item_index;
		
		return GetMenuItemCommandCode( menu_choice );
	}
	
	void SetMenuItemCommandCode( long menu_choice, CommandCode code );
	
	inline
	void SetMenuItemCommandCode( MenuID         menu_id,
	                             MenuItemIndex  item_index,
	                             CommandCode    code )
	{
		long menu_choice = menu_id << 16 | item_index;
		
		SetMenuItemCommandCode( menu_choice, code );
	}
	
}

#endif
