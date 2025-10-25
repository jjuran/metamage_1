/*
	wait_next_event.cc
	------------------
*/

#include "mac_app/wait_next_event.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// mac-sys-utils
#include "mac_sys/has/WaitNextEvent.hh"


#ifdef TARGET_CPU_68K
#define STATIC_68K  static
#else
#define STATIC_68K  /**/
#endif


namespace mac {
namespace app {

#if __LP64__

int dummy;

#else

Boolean wait_next_event( EventMask event_mask, EventRecord& event )
{
	STATIC_68K const bool has_WNE = mac::sys::has_WaitNextEvent();
	
	if ( has_WNE )
	{
		return WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL );
	}
	
#if CALL_NOT_IN_CARBON
	
	SystemTask();
	
#endif
	
	return GetNextEvent( everyEvent, &event );
}

#endif

}
}
