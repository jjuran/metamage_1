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
#include "Nitrogen/RefCon.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	//typedef bool ( *MenuItemHandler )( MenuItemCode code );
	
	class MenuItemHandler
	{
		public:
			virtual bool Run( MenuItemCode code ) const = 0;
	};
	
	class MenuBar
	{
		public:
			MenuBar( const MenuItemHandler& handler );
			
			void AddMenu     ( N::MenuID menuID );
			void AddAppleMenu( N::MenuID menuID );
			
			void ProcessMenuItem( N::MenuID menuID, SInt16 item );
			void ProcessMenuItem( int menuItem );
		
		private:
			typedef std::map< N::MenuID, std::vector< MenuItemCode > > Menus;
			
			const MenuItemHandler& handler;
			Menus myMenus;
			N::MenuID appleMenuID;
	};
	
}

#endif

