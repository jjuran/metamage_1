/*
	map_anonymous.hh
	----------------
*/

#ifndef GENIE_MMAP_MAPANONYMOUS_HH
#define GENIE_MMAP_MAPANONYMOUS_HH

// vfs
#include "vfs/memory_mapping_ptr.hh"


namespace Genie
{
	
	vfs::memory_mapping_ptr map_anonymous( std::size_t length, int prot, int flags );
	
}

#endif

