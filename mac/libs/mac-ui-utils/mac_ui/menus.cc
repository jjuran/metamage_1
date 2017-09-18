/*
	menus.hh
	--------
*/

#include "mac_ui/menus.hh"


namespace mac {
namespace ui  {

void delete_all_menu_items( MenuRef menu )
{
	uint16_t n = CountMenuItems( menu );
	
	while ( n > 0 )
	{
		DeleteMenuItem( menu, n );
		
		--n;
	}
}

}
}
