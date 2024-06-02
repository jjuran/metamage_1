/*
	trap_available.cc
	-----------------
	
	This logic is based on sample code from Inside Macintosh: Overview.
	See Listing 8-1: "Determining if a system software is available".
*/

#include "mac_sys/trap_available.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifdef __MACOS__
#ifndef __TRAPS__
#include <Traps.h>
#endif
#endif

// mac-sys-utils
#if ! TARGET_API_MAC_CARBON
#include "mac_sys/trap_address.hh"
#endif


namespace mac {
namespace sys {
	
#if ! TARGET_API_MAC_CARBON
	
	static inline
	bool fewer_traps()
	{
		return TARGET_CPU_68K  &&
		       get_trap_address( 0xA86E ) ==
		       get_trap_address( 0xAA6E );
	}
	
	static inline
	bool out_of_bounds( unsigned short trap )
	{
		return trap >= 0xAA00  &&  fewer_traps();
	}
	
	static inline
	UniversalProcPtr unimplemented()
	{
		return get_trap_address( _Unimplemented );
	}
	
	bool trap_available( unsigned short trap )
	{
		if ( out_of_bounds( trap ) )
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
