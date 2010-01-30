/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// Nitrogen
#include "Nitrogen/Menus.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	void AddMenu( Nitrogen::MenuID menuID );
	
	void AddAppleMenu( Nitrogen::MenuID menuID );
	
	MenuItemCode HandleMenuItem( Nitrogen::MenuID menuID, SInt16 item );
	
	
	class MenuItemHandler
	{
		public:
			virtual bool Run( MenuItemCode code ) const = 0;
	};
	
}

#endif

