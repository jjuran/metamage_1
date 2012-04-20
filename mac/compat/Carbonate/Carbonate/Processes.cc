// Carbonate/Processes.cc


#ifndef __PROCESSES__
#include <Processes.h>
#endif

#if !TARGET_API_MAC_CARBON

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
//#include "Carbonate/Files.hh"
#endif

// These functions are always declared in the headers and are always extern.

OSStatus GetProcessBundleLocation( const ProcessSerialNumber* psn, FSRef* result )
{
	ProcessInfoRec info = { 0 };
	
	FSSpec appSpec;
	
	info.processInfoLength = sizeof info;
	info.processAppSpec = &appSpec;
	
	OSStatus err = GetProcessInformation( psn, &info );
	
	if ( err == noErr )
	{
		err = FSpMakeFSRef( &appSpec, result );
	}
	
	return err;
}

#endif

