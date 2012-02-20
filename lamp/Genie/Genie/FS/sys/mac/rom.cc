/*
	Genie/FS/sys/mac/rom.cc
	-----------------------
*/

#include "Genie/FS/sys/mac/rom.hh"

// POSIX
#include "sys/stat.h"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/MemoryFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static UInt32 get_ROM_size()
	{
		SInt32 rom_size;
		
		const OSErr err = ::Gestalt( gestaltROMSize, &rom_size );
		
		return err == noErr ? rom_size : 0;
	}
	
	
	static const UInt32 global_rom_size = get_ROM_size();
	
	
	static off_t mac_rom_geteof( const FSTree* node )
	{
		return global_rom_size;
	}
	
	static IOPtr mac_rom_open( const FSTree* node, int flags, mode_t mode )
	{
	#if TARGET_API_MAC_CARBON
		
		throw p7::errno_t( EPERM );
		
	#else
		
		return new MemoryFileHandle( node,
		                             flags,
		                             LMGetROMBase(),
		                             global_rom_size );
		
	#endif
	}
	
	static const data_method_set mac_rom_data_methods =
	{
		&mac_rom_open,
		&mac_rom_geteof
	};
	
	static const node_method_set mac_rom_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&mac_rom_data_methods
	};
	
	FSTreePtr New_FSTree_sys_mac_rom( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		const mode_t mode = !TARGET_API_MAC_CARBON ? S_IFREG | 0400
		                  : global_rom_size != 0   ? S_IFREG
		                  :                          0;
		
		return new FSTree( parent, name, mode, &mac_rom_methods );
	}
	
}

