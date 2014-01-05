/*
	Genie/FS/sys/mac/tempmem.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/tempmem.hh"

// POSIX
#include <sys/stat.h>

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"

// MacVFS
#include "MacVFS/mmap/Handle_memory_mapping.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static memory_mapping_ptr
	//
	tempmem_mmap( vfs::filehandle* that, size_t length, int prot, int flags, off_t offset )
	{
		if ( offset != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		n::owned< N::Handle > h = N::TempNewHandle( length );
		
		return new vfs::Handle_memory_mapping( h, length, flags );
	}
	
	static const vfs::general_method_set tempmem_general_methods =
	{
		&tempmem_mmap,
	};
	
	static const vfs::filehandle_method_set tempmem_methods =
	{
		NULL,
		NULL,
		NULL,
		&tempmem_general_methods,
	};
	
	
	static off_t mac_tempmem_geteof( const FSTree* that )
	{
		return ::TempFreeMem();
	}
	
	static IOPtr mac_tempmem_open( const FSTree* that, int flags, mode_t mode )
	{
		return new vfs::filehandle( that, flags, &tempmem_methods );
		
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
	
	FSTreePtr New_FSTree_sys_mac_tempmem( const FSTree*        parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new FSTree( parent, name, S_IFREG | 0, &mac_tempmem_methods );
	}
	
}

