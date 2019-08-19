/*
	ColorQuickdraw.cc
	-----------------
*/

#include "MacFeatures/ColorQuickdraw.hh"

// Mac OS
#ifndef __APPLE__
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace MacFeatures
{
	
#if TARGET_CPU_68K  &&  !__MC68020__
	
	bool Has_ColorQuickdraw()
	{
		enum { _SysEnvirons = 0xA090 };
		
		if ( ! mac::sys::trap_available( _SysEnvirons ) )
		{
			return false;
		}
		
		SysEnvRec env;
		
		SysEnvirons( curSysEnvVers, &env );
		
		return env.hasColorQD;
	}
	
#endif
	
}
