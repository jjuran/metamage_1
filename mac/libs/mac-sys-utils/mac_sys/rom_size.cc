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


#ifdef __MC68K__

short ROM85 : 0x028E;  // ffff -> 64K, 7fff -> 128K, 3fff -> 256K

#else

static short ROM85;

#endif

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
		else if ( TARGET_CPU_68K  &&  rom_size == 0 )
		{
			return ROM85      < 0 ?  64 * 1024 :
			       ROM85 & 0x4000 ? 128 * 1024 :
			                        256 * 1024;
		}
		
		return rom_size;
	}
	
}
}
