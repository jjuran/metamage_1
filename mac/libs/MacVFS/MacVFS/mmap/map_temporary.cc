/*
	map_temporary.cc
	----------------
*/

#include "vfs/mmap/functions/map_temporary.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// MacVFS
#include "MacVFS/mmap/Handle_memory_mapping.hh"


namespace vfs
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	memory_mapping_ptr map_temporary( std::size_t length, int prot, int flags )
	{
		n::owned< Mac::Handle > handle = N::TempNewHandle( length );
		
		return new Handle_memory_mapping( handle, length, flags );
	}
	
}

