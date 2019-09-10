/*
	WindowMenu.hh
	-------------
*/

#ifndef PEDESTAL_WINDOWMENU_HH
#define PEDESTAL_WINDOWMENU_HH

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


namespace Pedestal
{
	
	void populate_Window_menu( MenuRef menu );
	
	WindowRef get_nth_window( int i );
	
}

#endif
