/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// Standard C++
#include <map>
#include <vector>

// Nitrogen
#include "Nitrogen/Menus.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	class MenuItemHandler
	{
		public:
			virtual bool Run( MenuItemCode code ) const = 0;
	};
	
	class MenuBar
	{
		public:
			MenuBar( const MenuItemHandler& handler );
			~MenuBar();
			
			void AddMenu     ( Nitrogen::MenuID menuID );
			void AddAppleMenu( Nitrogen::MenuID menuID );
			
			void ProcessMenuItem( Nitrogen::MenuID menuID, SInt16 item );
			void ProcessMenuItem( int menuItem );
		
		private:
			typedef std::map< Nitrogen::MenuID, std::vector< MenuItemCode > > Menus;
			
			const MenuItemHandler& handler;
			Menus myMenus;
			Nitrogen::MenuID appleMenuID;
	};
	
}

#endif

