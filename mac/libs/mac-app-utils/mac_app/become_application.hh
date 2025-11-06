/*
	become_application.hh
	---------------------
*/

#ifndef MACAPP_BECOMEAPPLICATION_HH
#define MACAPP_BECOMEAPPLICATION_HH

// Mac OS X
#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif


namespace mac {
namespace app {

bool launched_as_application();

void become_application_Carbon();
void become_application_Cocoa();

inline
int become_application()
{
	if ( launched_as_application() )
	{
		return -1;  // already an application
	}
	
#if defined( MAC_OS_X_VERSION_10_6 )  &&  defined( __OBJC__ )
	
	become_application_Cocoa();
	
#elif defined( MAC_OS_X_VERSION_10_3 )
	
	become_application_Carbon();
	
#else
	
	return 0;  // can't become an application
	
#endif
	
	return 1;  // became an application
}

}
}

#endif
