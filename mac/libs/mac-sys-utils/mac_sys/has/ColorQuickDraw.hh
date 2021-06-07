/*
	ColorQuickDraw.hh
	-----------------
*/

#ifndef MACSYS_HAS_COLORQUICKDRAW_HH
#define MACSYS_HAS_COLORQUICKDRAW_HH

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
#ifdef __MC68K__
	
	/*
		Calling Gestalt() or even SysEnvirons already requires a trap check,
		so just trap-check Color QuickDraw itself.
	*/
	
	inline
	bool has_ColorQuickDraw()
	{
		enum
		{
			_GetCWMgrPort = 0xAA48,
		};
		
		return trap_available( _GetCWMgrPort );
	}
	
#else
	
	inline
	unsigned char has_ColorQuickDraw()
	{
		return true;
	}
	
#endif
	
}
}

#endif
