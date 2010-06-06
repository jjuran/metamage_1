/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// Nitrogen
#include "Mac/Menus/Types/MenuID.hh"

// Pedestal
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	void AddMenu( Mac::MenuID menuID );
	
	void PopulateAppleMenu( Mac::MenuID menuID );
	
	MenuItemCode HandleMenuItem( Mac::MenuID menuID, SInt16 item );
	
}

#endif

