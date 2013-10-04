/*
	get_machine_name.cc
	-------------------
*/

#include "mac_sys/get_machine_name.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace mac {
namespace sys {
	
	const unsigned char* get_machine_name()
	{
		SInt32 mnam;
		
		if ( OSErr err = ::Gestalt( gestaltUserVisibleMachineName, &mnam ) )
		{
			return NULL;
		}
		
		ConstStr255Param result = *(ConstStr255Param*) &mnam;
		
		return result;
	}
	
}
}

