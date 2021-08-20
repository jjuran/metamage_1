/*
	menus.cc
	--------
*/

#include "menus.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"


MenuRef Edit_menu;
MenuRef Options_menu;

#ifdef __MC68K__

UInt8 SdVolume : 0x0260;
UInt8 SdEnable : 0x0261;

#else

const UInt8 SdVolume = 0;
const UInt8 SdEnable = 0;

#endif

void set_up_menus()
{
	Edit_menu    = GetMenuHandle( Edit    );
	Options_menu = GetMenuHandle( Options );
}
