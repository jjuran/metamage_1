/*
	menus.cc
	--------
*/

#include "menus.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"

// Tic-tac-toe
#include "sound.hh"


#define LOWMEM( addr, type )  (*(type*) (addr))

#define SdVolume  LOWMEM( 0x0260, Byte )
#define SdEnable  LOWMEM( 0x0261, Byte )


MenuRef Edit_menu;
MenuRef Options_menu;

static
void set_up_Options_menu()
{
	if ( ! TARGET_API_MAC_CARBON  &&  SdVolume > 0 )
	{
		sound_enabled = true;
		
		CheckMenuItem( Options_menu, Sound, sound_enabled );
	}
	else if ( TARGET_API_MAC_CARBON  ||  ! SdEnable )
	{
		mac::ui::disable_menu_item( Options_menu, Sound );
	}
}

void set_up_menus()
{
	Edit_menu    = GetMenuHandle( Edit    );
	Options_menu = GetMenuHandle( Options );
	
	set_up_Options_menu();
}
