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
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


namespace Pedestal
{
	
	void AddMenu( MenuRef menu );
	
	void PopulateAppleMenu( MenuRef menu );
	
	CommandCode HandleMenuItem( Mac::MenuID menuID, SInt16 item );
	
}

#endif

