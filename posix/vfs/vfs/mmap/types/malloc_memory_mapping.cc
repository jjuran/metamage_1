/*
	malloc_memory_mapping.cc
	------------------------
*/

#include "vfs/mmap/types/malloc_memory_mapping.hh"

// Standard C
#include <stdlib.h>


namespace vfs
{
	
	malloc_memory_mapping::~malloc_memory_mapping()
	{
		free( get_address() );
	}
	
}

