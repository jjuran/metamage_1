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
	
	
	CommandCode GetMenuItemCommandCode( long menu_choice )
	{
		Menus::const_iterator it = gMenus.find( menu_choice );
		
		if ( it != gMenus.end() )
		{
			return (CommandCode) it->second;
		}
		
		return CommandCode();
	}
	
	void SetMenuItemCommandCode( long menu_choice, CommandCode code )
	{
		gMenus[ menu_choice ] = code;
	}
	
}
