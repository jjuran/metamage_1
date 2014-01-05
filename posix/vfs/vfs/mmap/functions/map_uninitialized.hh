/*
	map_uninitialized.hh
	--------------------
*/

#ifndef VFS_MMAP_MAPUNINITIALIZED_HH
#define VFS_MMAP_MAPUNINITIALIZED_HH

// vfs
#include "vfs/memory_mapping_ptr.hh"


namespace vfs
{
	
	memory_mapping_ptr map_uninitialized( std::size_t length, int prot, int flags );
	
}

#endif

