/*
	about_box.hh
	------------
*/

#ifndef MACAPP_ABOUTBOX_HH
#define MACAPP_ABOUTBOX_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-app-utils
#include "mac_app/QDAboutBox.hh"


namespace mac {
namespace app {
	
	enum
	{
		kAboutWindowKind = 256,
	};
	
	void show_About_box( OSType creator );
	
	bool close_About_box( WindowRef window );
	
}
}

#endif
