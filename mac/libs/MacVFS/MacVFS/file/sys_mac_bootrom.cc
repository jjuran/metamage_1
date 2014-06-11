/*
	sys_mac_bootrom.cc
	------------------
*/

#include "MacVFS/file/sys_mac_bootrom.hh"

// POSIX
#include <sys/stat.h>

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


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	enum
	{
		gestaltMachineType = 'mach',
		
		gestaltPowerMacNewWorld = 406,
	};
	
	char* const global_bootrom_addr = (char*) 0xFFF00000;
	const off_t global_bootrom_size = 1024 * 1024;  // 1 MiB
	
	
	static off_t mac_bootrom_geteof( const node* that )
	{
		return global_bootrom_size;
	}
	
	static filehandle_ptr mac_bootrom_open( const node* that, int flags, mode_t mode )
	{
		return open_buffer_file( *that,
		                         flags,
		                         global_bootrom_addr,
		                         global_bootrom_size );
	}
	
	static const data_method_set mac_bootrom_data_methods =
	{
		&mac_bootrom_open,
		&mac_bootrom_geteof
	};
	
	static const node_method_set mac_bootrom_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&mac_bootrom_data_methods
	};
	
	node_ptr new_sys_mac_bootrom( const node*          parent,
	                              const plus::string&  name,
	                              const void*          args )
	{
		const unsigned long mach = mac::sys::gestalt( gestaltMachineType );
		
		if ( mach != gestaltPowerMacNewWorld )
		{
			p7::throw_errno( ENOENT );
		}
		
		const mode_t mode = S_IFREG | 0400;
		
		return new node( parent, name, mode, &mac_bootrom_methods );
	}
	
}
