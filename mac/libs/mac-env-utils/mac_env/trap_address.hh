/*
	trap_address.hh
	---------------
*/

#ifndef MACENV_TRAPADDRESS_HH
#define MACENV_TRAPADDRESS_HH

// Mac OS
#ifndef __PATCHES__
#include <Patches.h>
#endif


namespace mac {
namespace env {
	
	inline
	UniversalProcPtr get_trap_address( unsigned short trap )
	{
		return trap < 0xA800 ? GetOSTrapAddress     ( trap )
		                     : GetToolboxTrapAddress( trap );
	}
	
	inline
	void set_trap_address( UniversalProcPtr addr, unsigned short trap )
	{
		if ( trap < 0xA800 )
		{
			SetOSTrapAddress( addr, trap );
		}
		else
		{
			SetToolboxTrapAddress( addr, trap );
		}
	}
	
}
}

#endif
