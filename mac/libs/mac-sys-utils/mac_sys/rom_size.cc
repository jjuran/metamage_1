/*
	rom_size.cc
	-----------
*/

#include "mac_sys/rom_size.hh"

// Mac OS
#ifndef __TARGETCONDITIONALS__
#include <TargetConditionals.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	const unsigned long gestaltROMSize = 'rom ';
	
	const unsigned long gestaltMachineType = 'mach';
	
	const long gestaltMacClassic = 17;
	
	
	unsigned long rom_size()
	{
		long rom_size = gestalt( gestaltROMSize );
		
		if ( rom_size == 3 * 1024 * 1024 )
		{
			rom_size = 4 * 1024 * 1024;
		}
		else if ( TARGET_CPU_68K  &&  rom_size == 256 * 1024 )
		{
			if ( gestalt( gestaltMachineType ) == gestaltMacClassic )
			{
				rom_size = 512 * 1024;  // special case for Mac Classic
			}
		}
		
		return rom_size;
	}
	
}
}
