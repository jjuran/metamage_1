/*
	map_anonymous.hh
	----------------
*/

#ifndef MACVFS_MMAP_MAPANONYMOUS_HH
#define MACVFS_MMAP_MAPANONYMOUS_HH

// vfs
#include "vfs/memory_mapping_ptr.hh"


namespace vfs
{
	
	memory_mapping_ptr map_anonymous( std::size_t length, int prot, int flags );
	
}

#endif

