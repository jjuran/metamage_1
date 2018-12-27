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

void install_menus()
{
	short id = 128;
	
	while ( MenuRef menu = GetMenu( id++ ) )
	{
		InsertMenu( menu, 0 );
	}
	
	DrawMenuBar();
}

}
}
