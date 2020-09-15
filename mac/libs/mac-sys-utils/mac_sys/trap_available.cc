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
#ifndef __PATCHES__
#include <Patches.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif
#endif


namespace mac {
namespace sys {
	
#if ! TARGET_API_MAC_CARBON
	
	static inline
	bool fewer_traps()
	{
		return TARGET_CPU_68K  &&
		       GetToolboxTrapAddress( 0xA86E ) ==
		       GetToolboxTrapAddress( 0xAA6E );
	}
	
	static inline
	bool out_of_bounds( unsigned short trap )
	{
		return trap & 0x800  &&  (trap & 0x3FF) >= 0x200  &&  fewer_traps();
	}
	
	static inline
	TrapType get_trap_type( unsigned short trap )
	{
		return trap & 0x800 ? ToolTrap : OSTrap;
	}
	
	static inline
	UniversalProcPtr unimplemented()
	{
		return GetToolboxTrapAddress( _Unimplemented );
	}
	
	bool trap_available( unsigned short trap )
	{
		if ( out_of_bounds( trap ) )
		{
			return false;
		}
		
		TrapType type = get_trap_type( trap );
		
		return NGetTrapAddress( trap, type ) != unimplemented();
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
