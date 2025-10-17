/*
	WaitNextEvent.hh
	----------------
*/

#ifndef MACSYS_HAS_WAITNEXTEVENT_HH
#define MACSYS_HAS_WAITNEXTEVENT_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_WaitNextEvent()
	{
		enum
		{
			_WaitNextEvent = 0xA860,
		};
		
		return ! TARGET_CPU_68K  ||  trap_available( _WaitNextEvent );
	}
	
}
}

#endif
