/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// Nitrogen
#include "Nitrogen/Menus.hh"

// Pedestal
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	void AddMenu( Nitrogen::MenuID menuID );
	
	void PopulateAppleMenu( Nitrogen::MenuID menuID );
	
	MenuItemCode HandleMenuItem( Nitrogen::MenuID menuID, SInt16 item );
	
}

#endif

