/*
	file_memory_mapping.cc
	----------------------
*/

#include "vfs/mmap/types/file_memory_mapping.hh"

// POSIX
#include <sys/mman.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/primitives/pwrite.hh"


namespace vfs
{
	
	file_memory_mapping::file_memory_mapping( memory_mapping*  memory,
	                                          filehandle*      file,
	                                          off_t            offset )
	:
		memory_mapping( memory->get_address(),
		                memory->get_size(),
		                memory->get_flags() ),
		its_memory( memory ),
		its_file  ( file   ),
		its_offset( offset )
	{
	}
	
	file_memory_mapping::~file_memory_mapping()
	{
		msync( get_address(), get_size(), MS_SYNC );
	}
	
	void file_memory_mapping::msync( void* addr, size_t len, int flags ) const
	{
		const int mmap_flags = get_flags();
		
		if ( (mmap_flags & (MAP_SHARED|MAP_PRIVATE)) == MAP_SHARED )
		{
			pwrite( *get_file(), (const char*) addr, len, get_offset() );
		}
	}
	
}

