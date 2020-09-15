/*
	Mac/Sound/Functions/SysBeep.cc
	------------------------------
*/

#include "Mac/Sound/Functions/SysBeep.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

#if ! __LP64__

// Mac OS

#ifndef __SOUND__
#include <Sound.h>
#endif


namespace Mac
{
	
	void SysBeep()
	{
		::SysBeep( 0 );  // should be 30 if we care about System 6
	}
	
}

#else

void dummy()
{
}

#endif  // #if ! __LP64__
