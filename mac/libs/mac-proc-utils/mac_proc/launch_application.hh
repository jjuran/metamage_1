/*
	launch_application.hh
	---------------------
*/

#ifndef MACPROC_LAUNCHAPPLICATION_HH
#define MACPROC_LAUNCHAPPLICATION_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

struct AppParameters;
struct FSSpec;
struct ProcessSerialNumber;


namespace mac  {
namespace proc {
	
	OSErr launch_application( ProcessSerialNumber*  psn,
	                          const FSSpec&         file,
	                          AppParameters*        params = NULL );
	
	inline
	OSErr launch_application( const FSSpec& file, AppParameters* params = NULL )
	{
		return launch_application( NULL, file, params );
	}
	
}
}

#endif
