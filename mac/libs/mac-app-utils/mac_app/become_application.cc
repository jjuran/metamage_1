/*
	become_application.cc
	---------------------
*/

#include "mac_app/become_application.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Standard C
#include <stdlib.h>
#include <string.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace mac {
namespace app {

bool launched_as_application()
{
	if ( const char* _ = getenv( "_" ) )
	{
		// We were launched from a shell (possibly via open).
		
		if ( memcmp( _, STR_LEN( "/usr/bin/open" ) ) != 0 )
		{
			// We were not launched via open.
			
			return false;
		}
	}
	
	return true;
}

void become_application_Carbon()
{
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	
#ifdef MAC_OS_X_VERSION_10_3
	
	TransformProcessType( &psn, kProcessTransformToForegroundApplication );
	
#endif
	
	SetFrontProcess( &psn );
}

}
}
