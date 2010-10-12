/*
	CursorDevices.cc
	----------------
*/

#include "MacFeatures/CursorDevices.hh"

#if TARGET_CPU_68K

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif


namespace MacFeatures
{
	
	bool Has_CursorDevices()
	{
		return ::GetToolboxTrapAddress( _CursorDeviceDispatch ) !=
		       ::GetToolboxTrapAddress( _Unimplemented        );
	}
	
}

#endif  // #if TARGET_CPU_68K

