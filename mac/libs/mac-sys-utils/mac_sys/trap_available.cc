/*
	trap_available.cc
	-----------------
	
	This logic is based on sample code from Inside Macintosh: Overview.
	See Listing 8-1: "Determining if a system software is available".
*/

#include "mac_sys/trap_available.hh"

// mac-sys-utils
#if ! TARGET_API_MAC_CARBON
#include "mac_sys/trap_address.hh"
#endif


namespace mac {
namespace sys {
	
	enum
	{
		_InitGraf = 0xA86E,
		
		_Unimplemented = 0xA89F,
	};
	
#if ! TARGET_API_MAC_CARBON
	
	static inline
	bool fewer_traps()
	{
		return TARGET_CPU_68K  &&
		       get_trap_address( _InitGraf          ) ==
		       get_trap_address( _InitGraf | 0x0200 );
	}
	
	static inline
	UniversalProcPtr unimplemented()
	{
		return get_trap_address( _Unimplemented );
	}
	
	bool trap_available( unsigned short trap )
	{
		if ( trap >= 0xAA00  &&  fewer_traps() )
		{
			return false;
		}
		
		return get_trap_address( trap ) != unimplemented();
	}
	
#else
	
	extern "C"
	void TrapAvailable_is_unavailable_in_Carbon();  // not defined
	
	bool trap_available( unsigned short trap )
	{
		TrapAvailable_is_unavailable_in_Carbon();
		
		return false;
	}
	
#endif
	
}
}
