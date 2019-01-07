/*
	menus.hh
	--------
*/

#include "mac_app/menus.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif


namespace mac {
namespace app {

enum
{
	kAppleMenuID = 1,
};

void install_menus()
{
	short id = 128;
	
	while ( MenuRef menu = GetMenu( id++ ) )
	{
		InsertMenu( menu, 0 );
	}
	
	DrawMenuBar();
	
	if ( ! TARGET_API_MAC_CARBON )
	{
		const ResType kDeskAccessoryResourceType = 'DRVR';
		
		MenuRef appleMenu = GetMenuHandle( kAppleMenuID );
		
		AppendResMenu( appleMenu, kDeskAccessoryResourceType );
	}
}

}
}
