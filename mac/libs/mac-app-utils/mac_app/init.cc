/*
	init.cc
	-------
*/

#include "mac_app/init.hh"

// Mac OS
#if ! TARGET_API_MAC_CARBON
#ifndef __DIALOGS__
#include <Dialogs.h>
#endif
#endif
#ifndef __APPLE__
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#endif


namespace mac {
namespace app {

void init_toolbox()
{
#if ! TARGET_API_MAC_CARBON
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	TEInit();
	InitDialogs( NULL );
	
#endif
	
#ifndef __APPLE__
	
	/*
		Calling InitCursor() is required for Carbon in OS 9, but unneeded in
		OS X (as of 10.4, at least).  The declaration was removed from the
		10.7 SDK, so the call must be conditionally compiled.  Alternatively,
		we could provide the missing declaration, but then /that/ would have
		to be conditional, and the call would have no effect anyway.
	*/
	
	InitCursor();
	
	FlushEvents( everyEvent, 0 );
	
#endif
}

}
}
