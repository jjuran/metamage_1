/*
	CGAboutBox.hh
	-------------
*/

#ifndef MACAPP_CGABOUTBOX_HH
#define MACAPP_CGABOUTBOX_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace mac {
namespace app {
	
	WindowRef CGAboutBox_make();
	
	void CGAboutBox_close( WindowRef window );
	
}
}

#endif
