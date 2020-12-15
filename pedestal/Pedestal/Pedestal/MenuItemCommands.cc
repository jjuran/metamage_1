/*
	MenuItemCommands.cc
	-------------------
*/

#include "Pedestal/MenuItemCommands.hh"

// Standard C++
#include <map>
#include <vector>


namespace Pedestal
{
	
	typedef unsigned long Code;
	
	typedef std::map< Mac::MenuID, std::vector< Code > > Menus;
	
	
	static Menus gMenus;
	
	
	CommandCode GetMenuItemCommandCode( Mac::MenuID    menu_id,
	                                    MenuItemIndex  item_index )
	{
		Menus::const_iterator it = gMenus.find( menu_id );
		
		if ( it != gMenus.end() )
		{
			const std::vector< Code >& commands = it->second;
			
			if ( item_index < commands.size() )
			{
				return (CommandCode) commands[ item_index ];
			}
		}
		
		return CommandCode();
	}
	
	void SetMenuItemCommandCode( Mac::MenuID    menu_id,
	                             MenuItemIndex  item_index,
	                             CommandCode    code )
	{
		std::vector< Code >& commands = gMenus[ menu_id ];
		
		if ( item_index >= commands.size() )
		{
			commands.resize( item_index + 1 );
		}
		
		commands[ item_index ] = code;
	}
	
}
