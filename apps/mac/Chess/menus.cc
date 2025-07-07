/*
	menus.cc
	--------
*/

#include "menus.hh"

// mac-sys-utils
#include "mac_sys/trap_available.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"

// Chess
#include "board.hh"


#if TARGET_CPU_68K
#define EXISTS(name)  ::mac::sys::trap_available(_##name)
#else
#define EXISTS(name)  true
#endif

enum
{
	_BitMapToRegion = 0xA8D7,
};


MenuRef Options_menu;

static
void set_up_Options_menu()
{
	/*
		Live Dragging refers to animating the actual chessman
		being dragged within the chess board.  The alternative
		is to merely draw an outline.
		
		Drawing an outline depends on BitMapToRegion() to compute
		the region, and therefore is disabled if it's unimplemented.
	*/
	
	live_dragging_enabled = true;
	
	if ( ! EXISTS( BitMapToRegion ) )
	{
		mac::ui::disable_menu_item( Options_menu, LiveDragging );
	}
}

void set_up_menus()
{
	Options_menu = GetMenuHandle( Options );
	
	set_up_Options_menu();
}
