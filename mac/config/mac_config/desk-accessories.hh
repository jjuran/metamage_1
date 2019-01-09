/*
	desk-accessories.hh
	-------------------
*/

#ifndef MACCONFIG_DESKACCESSORIES_HH
#define MACCONFIG_DESKACCESSORIES_HH

// mac-config
#include "mac_config/system.hh"


/*
	Desk accessories existed from the very beginning.
	In MultiFinder, desk accessories open in DA Handler by default, but can
	still be opened in the caller's process by holding Option.
	
	In System 7 and later, desk accessories run in their own processes.
	Applications still need to call OpenDeskAcc() to support launching Apple
	menu items (until Carbon, that is), but they no longer need to handle
	inSysWindow and such.
	
	Note that the logic is inverted here.  Usually we check for whether the
	system provides some facility, which tends to be guaranteed in or after a
	particular system version.  In this case, the topic is not an OS feature
	but an application responsibility.  In a sense, the feature is not having
	the burden of implementing DA support (true if the application requires
	System 7), but in order to let us write `if (CONFIG_DESK_ACCESSORIES)`
	rather than `if (! CONFIG_NO_DESK_ACCESSORIES)`, we negate the meaning.
*/

#ifndef CONFIG_DESK_ACCESSORIES
#define CONFIG_DESK_ACCESSORIES  (! CONFIG_SYSTEM_7_REQUIRED)
#endif

/*
	There is no CONFIG_DESK_ACCESSORIES_GRANTED, because there's no feature
	check to elide.  Desk accessories have always existed, so SystemClick()
	and such are always available.  If we support desk accessories at all,
	there's nothing else to check.
*/

#endif
