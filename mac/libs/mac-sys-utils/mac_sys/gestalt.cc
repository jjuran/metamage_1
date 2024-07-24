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

// mac-sys-utils
#include "mac_sys/has/Gestalt.hh"


namespace mac {
namespace sys {

long gestalt( unsigned long selector, long default_value )
{
	if ( ! has_Gestalt() )
	{
		return default_value;
	}
	
	SInt32 result;
	
	const OSErr err = ::Gestalt( selector, &result );
	
	return err == noErr ? result : default_value;
}

bool gestalt_defined( unsigned long selector )
{
	if ( ! has_Gestalt() )
	{
		return false;
	}
	
	SInt32 unused_Gestalt_result;
	
	const OSErr err = ::Gestalt( selector, &unused_Gestalt_result );
	
	return err == noErr;
}

}
}
