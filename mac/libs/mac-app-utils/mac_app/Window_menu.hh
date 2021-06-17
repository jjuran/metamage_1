/*
	Window_menu.hh
	--------------
*/

#ifndef MACAPP_WINDOWMENU_HH
#define MACAPP_WINDOWMENU_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif


namespace mac {
namespace app {
	
	void Window_menu_insert( WindowRef window );
	void Window_menu_remove( WindowRef window );
	
	void populate_Window_menu( MenuRef menu );
	
	WindowRef get_nth_window( int i );
	
}
}

#endif
