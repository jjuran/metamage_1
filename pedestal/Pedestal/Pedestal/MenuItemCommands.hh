/*
	MenuItemCommands.hh
	-------------------
*/

#ifndef PEDESTAL_MENUITEMCOMMANDS_HH
#define PEDESTAL_MENUITEMCOMMANDS_HH

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


namespace Pedestal
{
	
	CommandCode GetMenuItemCommandCode( long menu_choice );
	
	void SetMenuItemCommandCode( long menu_choice, CommandCode code );
	
}

#endif
