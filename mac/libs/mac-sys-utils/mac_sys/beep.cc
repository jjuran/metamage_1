/*
	beep.cc
	-------
*/

#include "mac_sys/beep.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#if ! __LP64__
#ifndef __SOUND__
#include <Sound.h>
#endif
#endif


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
