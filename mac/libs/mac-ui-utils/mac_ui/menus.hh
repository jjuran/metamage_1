/*
	menus.hh
	--------
*/

#ifndef MACUI_MENUS_HH
#define MACUI_MENUS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif


namespace mac {
namespace ui  {
	
	void delete_all_menu_items( MenuRef menu );
	
}
}

#endif
