/*
	Mac/Sound/Functions/SysBeep.cc
	------------------------------
*/

#include "Mac/Sound/Functions/SysBeep.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

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

