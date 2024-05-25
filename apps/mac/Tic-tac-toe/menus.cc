/*
	menus.cc
	--------
*/

#include "menus.hh"

// mac-sys-utils
#include "mac_sys/has/lowmem.hh"

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
	using mac::sys::has_lowmem;
	
	const bool lowmem_present = has_lowmem();
	
	if ( lowmem_present  &&  SdVolume > 0 )
	{
		sound_enabled = true;
		
		CheckMenuItem( Options_menu, Sound, sound_enabled );
	}
	else if ( ! lowmem_present  ||  ! SdEnable )
	{
		mac::ui::disable_menu_item( Options_menu, Sound );
	}
}

void set_up_menus()
{
	Edit_menu    = GetMenuHandle( Edit    );
	Options_menu = GetMenuHandle( Options );
	
	if ( TARGET_API_MAC_CARBON )
	{
		/*
			The Copy menu item is enabled in the 'MENU' resource,
			but the function isn't implemented in Carbon.
		*/
		
		mac::ui::disable_menu_item( Edit_menu, Copy );
	}
	
	set_up_Options_menu();
}
