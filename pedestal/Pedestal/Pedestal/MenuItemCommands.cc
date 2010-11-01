/*
	MenuItemCommands.cc
	-------------------
*/

#include "Pedestal/MenuItemCommands.hh"

// Standard C++
#include <map>
#include <vector>

// Nitrogen
//#include "Nitrogen/Menus.hh"


namespace Pedestal
{
	
	typedef std::map< Mac::MenuID, std::vector< CommandCode > > Menus;
	
	
	static Menus gMenus;
	
	
	CommandCode GetMenuItemCommandCode( Mac::MenuID    menu_id,
	                                    MenuItemIndex  item_index )
	{
		Menus::const_iterator it = gMenus.find( menu_id );
		
		if ( it != gMenus.end() )
		{
			const std::vector< CommandCode >& commands = it->second;
			
			if ( item_index < commands.size() )
			{
				CommandCode code = commands[ item_index ];
				
				return code;
			}
		}
		
		return CommandCode();
	}
	
	void SetMenuItemCommandCode( Mac::MenuID    menu_id,
	                             MenuItemIndex  item_index,
	                             CommandCode    code )
	{
		std::vector< CommandCode >& commands = gMenus[ menu_id ];
		
		if ( item_index >= commands.size() )
		{
			commands.resize( item_index + 1 );
		}
		
		commands[ item_index ] = code;
	}
	
}

