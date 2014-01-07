/*
	map_file.hh
	-----------
*/

#ifndef VFS_MMAP_MAPFILE_HH
#define VFS_MMAP_MAPFILE_HH

// vfs
#include "vfs/filehandle_fwd.hh"
#include "vfs/memory_mapping_ptr.hh"


namespace vfs
{
	
	memory_mapping_ptr map_file( filehandle& file, std::size_t length, int prot, int flags, off_t offset );
	
}

#endif

