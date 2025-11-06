/*
	become_application.mm
	---------------------
*/

#include "mac_app/become_application.hh"

// Mac OS X
#ifdef __APPLE__
#include <AppKit/AppKit.h>
#endif


namespace mac {
namespace app {

void become_application_Cocoa()
{
#ifdef MAC_OS_X_VERSION_10_6
	
	[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
	
#endif
	
	[NSApp activateIgnoringOtherApps: YES];
}

}
}
