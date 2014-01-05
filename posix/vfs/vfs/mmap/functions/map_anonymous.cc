/*
	map_anonymous.cc
	----------------
*/

#include "vfs/mmap/functions/map_anonymous.hh"

// Standard C
#include <string.h>

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/mmap/functions/map_uninitialized.hh"


namespace vfs
{
	
	memory_mapping_ptr map_anonymous( std::size_t length, int prot, int flags )
	{
		memory_mapping_ptr result = map_uninitialized( length, prot, flags );
		
		memset( result->get_address(), '\0', length );
		
		return result;
	}
	
}

