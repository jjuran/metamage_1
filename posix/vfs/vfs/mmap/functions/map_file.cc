/*
	map_file.cc
	-----------
*/

#include "vfs/mmap/functions/map_file.hh"

// Standard C
#include <string.h>

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/mmap/functions/map_uninitialized.hh"
#include "vfs/mmap/types/file_memory_mapping.hh"


namespace vfs
{
	
	memory_mapping_ptr map_file( filehandle& file, std::size_t length, int prot, int flags, off_t offset )
	{
		vfs::memory_mapping_ptr result = vfs::map_uninitialized( length, prot, flags );
		
		vfs::memory_mapping* mapping = result.get();
		
		result = vfs::memory_mapping_ptr( new vfs::file_memory_mapping( mapping, &file, offset ) );
		
		void* addr = mapping->get_address();
		
		ssize_t count = pread( file, (char*) addr, length, offset );
		
		if ( count < length )
		{
			memset( (char*) addr + count, '\0', length - count );
		}
		
		return result;
	}
	
}

