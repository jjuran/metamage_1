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
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// mac-sys-utils
#include "mac_sys/rom_size.hh"
#include "mac_sys/trap_available.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/types/buffer_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


#define ROM85    *(short*) 0x028E
#define ROMBase  *(char**) 0x02AE


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static inline
	char* LMGetROMBase()
	{
		return TARGET_API_MAC_CARBON ? 0 : ROMBase;
	}
	
	static
	off_t get_rom_size()
	{
		off_t size = mac::sys::rom_size();
		
		if ( TARGET_CPU_68K  &&  size == 0 )
		{
			enum { _CountADBs = 0xA077 };
			
			using mac::sys::trap_available;
			
			return + trap_available( _CountADBs ) ? 256 * 1024
			       : ROM85 > 0                    ? 128 * 1024
			       :                                64 * 1024;
		}
		
		return size;
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
