/*
	MenuItemCommands.cc
	-------------------
*/

#include "Pedestal/MenuItemCommands.hh"

// Standard C++
#include <map>


namespace Pedestal
{
	
	typedef unsigned long Code;
	
	typedef std::map< long, Code > Menus;
	
	
	static Menus gMenus;
	
	
	CommandCode GetMenuItemCommandCode( MenuID         menu_id,
	                                    MenuItemIndex  item_index )
	{
		long menu_choice = menu_id << 16 | item_index;
		
		Menus::const_iterator it = gMenus.find( menu_choice );
		
		if ( it != gMenus.end() )
		{
			return (CommandCode) it->second;
		}
		
		return CommandCode();
	}
	
	void SetMenuItemCommandCode( MenuID         menu_id,
	                             MenuItemIndex  item_index,
	                             CommandCode    code )
	{
		long menu_choice = menu_id << 16 | item_index;
		
		gMenus[ menu_choice ] = code;
	}
	
}
