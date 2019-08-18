/*
	ColorQuickdraw.cc
	-----------------
*/

#include "MacFeatures/ColorQuickdraw.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace MacFeatures
{
	
#if TARGET_CPU_68K  &&  !__MC68020__
	
	bool Has_ColorQuickdraw()
	{
		enum
		{
			gestaltQuickdrawVersion = 'qd  ',
			gestaltOriginalQD       = 0x0000,
		};
		
		// FIXME:  This always returns false if _Gestalt is unimplemented.
		
		long result;
		
		result = mac::sys::gestalt( gestaltQuickdrawVersion );
		
		return result != gestaltOriginalQD;
	}
	
#endif
	
}
