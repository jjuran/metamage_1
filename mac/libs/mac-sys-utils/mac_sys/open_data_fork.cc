/*
	open_data_fork.cc
	-----------------
*/

#include "mac_sys/open_data_fork.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifdef __MACOS__
#ifndef __FILES__
#include <Files.h>
#endif
#endif

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"


namespace mac {
namespace sys {

short open_data_fork( const FSSpec& file, signed char perm )
{
	OSErr err;
	short result;
	
#if ! __LP64__
	
	if ( mac::sys::has_FSSpec_calls() )
	{
		err = FSpOpenDF( &file, perm, &result );
	}
	else
	{
		err = HOpen( file.vRefNum, file.parID, file.name, perm, &result );
	}
	
	return err ? err : result;
	
#endif
	
	return 0;
}

}
}
