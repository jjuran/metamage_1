/*
	sys_mac_rom.cc
	--------------
*/

#include "MacVFS/file/sys_mac_rom.hh"

// POSIX
#include "sys/stat.h"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/types/buffer_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


#if TARGET_API_MAC_CARBON

static inline char* LMGetROMBase()  { return 0; }

#endif


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	const unsigned long gestaltROMSize = 'rom ';
	
	static off_t get_rom_size()
	{
		off_t rom_size = mac::sys::gestalt( gestaltROMSize );
		
		if ( rom_size == 3 * 1024 * 1024 )
		{
			rom_size = 4 * 1024 * 1024;
		}
		
		return rom_size;
	}
	
	static const off_t global_rom_size = get_rom_size();
	
	
	static off_t mac_rom_geteof( const node* that )
	{
		return global_rom_size;
	}
	
	static filehandle_ptr mac_rom_open( const node* that, int flags, mode_t mode )
	{
		if ( TARGET_API_MAC_CARBON )
		{
			throw p7::errno_t( EPERM );
		}
		
		return open_buffer_file( *that,
		                         flags,
		                         LMGetROMBase(),
		                         global_rom_size );
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
		&mac_rom_data_methods
	};
	
	node_ptr new_sys_mac_rom( const node*          parent,
	                          const plus::string&  name,
	                          const void*          args )
	{
		const mode_t mode = !TARGET_API_MAC_CARBON ? S_IFREG | 0400
		                  : global_rom_size != 0   ? S_IFREG
		                  :                          0;
		
		return new node( parent, name, mode, &mac_rom_methods );
	}
	
}

