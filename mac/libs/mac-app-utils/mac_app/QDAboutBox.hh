/*
	QDAboutBox.hh
	-------------
*/

#ifndef MACAPP_QDABOUTBOX_HH
#define MACAPP_QDABOUTBOX_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace mac {
namespace app {
	
	extern ConstStr255Param gAboutBoxTitle;
	extern ConstStr255Param gAboutBoxAppName;
	extern ConstStr255Param gAboutBoxAppVers;
	extern ConstStr255Param gAboutBoxAppCopy;
	
	WindowRef QDAboutBox_make();
	
	void QDAboutBox_close( WindowRef window );
	
}
}

#endif
