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
	
	inline
	void enable_menu_item( MenuRef menu, MenuItemIndex item )
	{
	#if CALL_NOT_IN_CARBON
		
		EnableItem( menu, item );
		return;
		
	#endif
		
		EnableMenuItem( menu, item );
	}
	
	inline
	void disable_menu_item( MenuRef menu, MenuItemIndex item )
	{
	#if CALL_NOT_IN_CARBON
		
		DisableItem( menu, item );
		return;
		
	#endif
		
		DisableMenuItem( menu, item );
	}
	
}
}

#endif
