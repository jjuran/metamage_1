/*
	MenuItemCommands.cc
	-------------------
*/

#include "Pedestal/MenuItemCommands.hh"

// plus
#include "plus/simple_map.hh"


namespace Pedestal
{
	
	typedef plus::simple_map< long, CommandCode > Menus;
	
	
	static Menus gMenus;
	
	
	CommandCode GetMenuItemCommandCode( long menu_choice )
	{
		Menus::const_iterator it = gMenus.find( menu_choice );
		
		if ( it )
		{
			return *it;
		}
		
		return CommandCode();
	}
	
	void SetMenuItemCommandCode( long menu_choice, CommandCode code )
	{
		gMenus[ menu_choice ] = code;
	}
	
}
