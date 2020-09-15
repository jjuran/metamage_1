/*
	event_handlers.hh
	-----------------
*/

#include "mac_app/event_handlers.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef MAC_OS_X_VERSION_10_5
// CoreServices in 10.4 lacks AppleEvents.h.
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-app-utils
#include "mac_app/state.hh"


namespace mac {
namespace app {

static
pascal OSErr Quit( const AppleEvent* event, AppleEvent* reply, SRefCon )
{
	quitting = true;
	
	return noErr;
}

void install_basic_event_handlers()
{
	DEFINE_UPP( AEEventHandler, Quit );
	
	OSErr err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEQuitApplication,
	                             UPP_ARG( Quit ),
	                             0,
	                             false );
}
	
}
}
