/*
	gestalt.cc
	----------
*/

#include "mac_sys/gestalt.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace mac {
namespace sys {
	
	long gestalt( unsigned long selector, long default_value )
	{
		SInt32 result;
		
		const OSErr err = ::Gestalt( selector, &result );
		
		return err == noErr ? result : default_value;
	}
	
}
}
