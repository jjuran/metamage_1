/*
	tempmem.cc
	----------
*/

#include "MacVFS/file/tempmem.hh"

// POSIX
#include <sys/stat.h>

// mac-sys-utils
#include "mac_sys/temp_free_mem.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/memory_mapping.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/mmap/functions/map_temporary.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static memory_mapping_ptr
	//
	tempmem_mmap( filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		if ( offset != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		return map_temporary( length, prot, flags );
	}
	
	static const general_method_set tempmem_general_methods =
	{
		&tempmem_mmap,
	};
	
	static const filehandle_method_set tempmem_methods =
	{
		NULL,
		NULL,
		NULL,
		&tempmem_general_methods,
	};
	
	
	static off_t mac_tempmem_geteof( const node* that )
	{
		return mac::sys::temp_free_mem();
	}
	
	static filehandle_ptr mac_tempmem_open( const node* that, int flags, mode_t mode )
	{
		return new filehandle( that, flags, &tempmem_methods );
		
	}
	
	static const data_method_set mac_tempmem_data_methods =
	{
		&mac_tempmem_open,
		&mac_tempmem_geteof
	};
	
	static const node_method_set mac_tempmem_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&mac_tempmem_data_methods
	};
	
	node_ptr new_sys_mac_tempmem( const node*          parent,
	                              const plus::string&  name,
	                              const void*          args )
	{
		return new node( parent, name, S_IFREG | 0, &mac_tempmem_methods );
	}
	
}
