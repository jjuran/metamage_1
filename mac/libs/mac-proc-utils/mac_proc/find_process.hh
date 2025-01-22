/*
	find_process.hh
	---------------
*/

#ifndef MACPROC_FINDPROCESS_HH
#define MACPROC_FINDPROCESS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

struct FSRef;
struct FSSpec;
struct ProcessSerialNumber;


namespace mac  {
namespace proc {
	
	OSErr test_process( const ProcessSerialNumber& psn );
	
	OSErr find_process( ProcessSerialNumber& psn, OSType signature );
	
	OSErr find_process( ProcessSerialNumber& psn, const FSRef&  app_file );
	OSErr find_process( ProcessSerialNumber& psn, const FSSpec& app_file );
	
}
}

#endif
