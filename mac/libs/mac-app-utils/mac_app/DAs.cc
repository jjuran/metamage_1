/*
	DAs.cc
	------
*/

#include "mac_app/DAs.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#if ! TARGET_API_MAC_CARBON
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace mac {
namespace app {

#if __LP64__

int dummy;

#else

bool close_DA_window( WindowRef window )
{
	const short kind = GetWindowKind( window );
	
	if ( kind < 0 )
	{
		#if CALL_NOT_IN_CARBON
		
		CloseDeskAcc( kind );
		
		#endif
	}
	
	return kind < 0;
}

bool close_front_DA()
{
	if ( WindowRef window = FrontWindow() )
	{
		return close_DA_window( window );
	}
	
	return false;
}

#endif  // #if __LP64__

}
}
