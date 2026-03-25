/*
	become_application.cc
	---------------------
*/

#include "mac_app/become_application.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// Standard C
#include <stdlib.h>
#include <string.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


/*
	This undocumented API call works in Mac OS X 10.2, at least.
*/

extern "C" OSErr CPSEnableForegroundOperation( ProcessSerialNumber*  psn,
                                               UInt32                _2,
                                               UInt32                _3,
                                               UInt32                _4,
                                               UInt32                _5 );

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
	
#else
	
	GetCurrentProcess( &psn );
	
	CPSEnableForegroundOperation( &psn, 0x03, 0x3C, 0x2C, 0x1103 );
	
#endif
	
	SetFrontProcess( &psn );
}

}
}
