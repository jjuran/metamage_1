/*
	Mac/Sound/Functions/SysBeep.cc
	------------------------------
*/

#include "Mac/Sound/Functions/SysBeep.hh"

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

