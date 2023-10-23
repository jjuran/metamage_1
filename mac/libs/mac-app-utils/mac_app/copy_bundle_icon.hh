/*
	copy_bundle_icon.hh
	-------------------
*/

#ifndef MACAPP_COPYBUNDLEICON_HH
#define MACAPP_COPYBUNDLEICON_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif


namespace mac {
namespace app {
	
	IconRef copy_bundle_icon();
	
}
}

#endif
