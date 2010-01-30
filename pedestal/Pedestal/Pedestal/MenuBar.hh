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
			
			void ProcessMenuItem( Nitrogen::MenuID menuID, SInt16 item );
			void ProcessMenuItem( int menuItem );
		
		private:
			const MenuItemHandler& handler;
	};
	
}

#endif

