/*
	file_memory_mapping.cc
	----------------------
*/

#include "vfs/mmap/types/file_memory_mapping.hh"

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	file_memory_mapping::file_memory_mapping( memory_mapping*  memory,
	                                          filehandle*      file )
	:
		memory_mapping( memory->get_address(),
		                memory->get_size(),
		                memory->get_flags() ),
		its_memory( memory ),
		its_file  ( file   )
	{
	}
	
	file_memory_mapping::~file_memory_mapping()
	{
	}
	
}

