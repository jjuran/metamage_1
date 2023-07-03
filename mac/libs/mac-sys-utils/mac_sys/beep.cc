/*
	beep.cc
	-------
*/

#include "mac_sys/beep.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// <Sound.h>
extern "C" pascal void SysBeep( short ticks )  ONEWORDINLINE( 0xA9C8 );


namespace mac {
namespace sys {

void beep()
{
#if ! __LP64__
	
	SysBeep( 6 );
	
#endif
}

}
}
