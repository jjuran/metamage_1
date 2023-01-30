/*
	menus.cc
	--------
*/

#include "menus.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"

// Tic-tac-toe
#include "sound.hh"

MenuRef Edit_menu;
MenuRef Options_menu;

#ifdef __MC68K__

UInt8 SdVolume : 0x0260;
UInt8 SdEnable : 0x0261;

#else

const UInt8 SdVolume = 0;
const UInt8 SdEnable = 0;

#endif

static
void set_up_Options_menu()
{
#if ! TARGET_API_MAC_CARBON
	
	if ( SdVolume > 0 )
	{
		sound_enabled = true;
		
		CheckMenuItem( Options_menu, Sound, sound_enabled );
	}
	else if ( ! SdEnable )
	
#endif
	
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
