/*
	map_temporary.hh
	----------------
*/

#ifndef VFS_MMAP_MAPTEMPORARY_HH
#define VFS_MMAP_MAPTEMPORARY_HH

// vfs
#include "vfs/memory_mapping_ptr.hh"


namespace vfs
{
	
	memory_mapping_ptr map_temporary( std::size_t length, int prot, int flags );
	
}

#endif

